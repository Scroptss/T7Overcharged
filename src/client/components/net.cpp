// Net: HTTP(S) requests from Lua that never block the game.
//
//   Net.Start( method, url [, body [, contentType [, bearerToken]]] ) -> id (nil if it can't start)
//   Net.Poll( id ) -> "pending" | "done" | "error", status code, body (or the error text)
//
// Requests run on their own thread through WinHTTP, so HTTPS works (Axios speaks plain
// http:// only and waits on the UI thread). Poll from a UITimer; a finished request is
// forgotten after the Poll that returns it. Up to 16 at once, responses up to 4 MB.
#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "havok/hks_api.hpp"
#include "havok/lua_api.hpp"

#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

namespace net
{
	struct job
	{
		std::atomic<int> state{ 0 };	// 0 pending, 1 done, 2 error
		int status = 0;
		std::string body;
	};

	std::mutex jobs_mutex;
	std::unordered_map<int, std::shared_ptr<job>> jobs;
	int next_id = 1;
	constexpr size_t max_jobs = 16;
	constexpr size_t max_body = 4 * 1024 * 1024;

	std::wstring widen(const std::string& s)
	{
		if (s.empty())
			return {};
		const int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
		std::wstring w(n, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), w.data(), n);
		return w;
	}

	void run(const std::shared_ptr<job> j, const std::string method, const std::string url, const std::string body, const std::string content_type, const std::string bearer)
	{
		HINTERNET session = nullptr, connection = nullptr, request = nullptr;
		std::string error;

		const std::wstring wurl = widen(url);
		wchar_t host[256] = {};
		wchar_t path[4096] = {};
		wchar_t extra[4096] = {};
		URL_COMPONENTS parts = {};
		parts.dwStructSize = sizeof(parts);
		parts.lpszHostName = host;
		parts.dwHostNameLength = static_cast<DWORD>(std::size(host));
		parts.lpszUrlPath = path;
		parts.dwUrlPathLength = static_cast<DWORD>(std::size(path));
		parts.lpszExtraInfo = extra;
		parts.dwExtraInfoLength = static_cast<DWORD>(std::size(extra));

		if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &parts))
			error = "bad url";
		if (error.empty())
		{
			session = WinHttpOpen(L"T7Overcharged", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (!session)
				error = "WinHttpOpen failed";
		}
		if (error.empty())
		{
			WinHttpSetTimeouts(session, 5000, 5000, 15000, 20000);
			connection = WinHttpConnect(session, host, parts.nPort, 0);
			if (!connection)
				error = "connect failed";
		}
		if (error.empty())
		{
			const std::wstring target = std::wstring(path) + extra;
			const DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
			request = WinHttpOpenRequest(connection, widen(method).c_str(), target.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
			if (!request)
				error = "open request failed";
		}
		if (error.empty())
		{
			std::wstring headers;
			if (!body.empty())
				headers = L"Content-Type: " + widen(content_type.empty() ? "application/json" : content_type) + L"\r\n";
			if (!bearer.empty())
				headers += L"Authorization: Bearer " + widen(bearer) + L"\r\n";
			const BOOL sent = WinHttpSendRequest(request,
				headers.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : headers.c_str(),
				headers.empty() ? 0 : static_cast<DWORD>(-1),
				body.empty() ? WINHTTP_NO_REQUEST_DATA : const_cast<char*>(body.data()),
				static_cast<DWORD>(body.size()), static_cast<DWORD>(body.size()), 0);
			if (!sent || !WinHttpReceiveResponse(request, nullptr))
				error = "request failed (" + std::to_string(GetLastError()) + ")";
		}
		if (error.empty())
		{
			DWORD code = 0;
			DWORD size = sizeof(code);
			WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &code, &size, WINHTTP_NO_HEADER_INDEX);
			j->status = static_cast<int>(code);

			DWORD available = 0;
			while (WinHttpQueryDataAvailable(request, &available) && available > 0)
			{
				std::string chunk(available, '\0');
				DWORD read = 0;
				if (!WinHttpReadData(request, chunk.data(), available, &read) || read == 0)
					break;
				j->body.append(chunk.data(), read);
				if (j->body.size() > max_body)
				{
					error = "response too large";
					break;
				}
			}
		}

		if (request)
			WinHttpCloseHandle(request);
		if (connection)
			WinHttpCloseHandle(connection);
		if (session)
			WinHttpCloseHandle(session);

		if (!error.empty())
			j->body = error;
		j->state = error.empty() ? 1 : 2;
	}

	std::string arg_string(lua::lua_State* s, int index)
	{
		if (hks::hksi_lua_gettop(s) < index)
			return {};
		const char* str = lua::lua_tostring(s, index);
		return str ? str : "";
	}

	int start(lua::lua_State* s)
	{
		const std::string method = arg_string(s, 1);
		const std::string url = arg_string(s, 2);
		if (method.empty() || url.empty())
		{
			lua::lua_pushnil(s);
			return 1;
		}

		auto j = std::make_shared<job>();
		int id;
		{
			std::lock_guard lock(jobs_mutex);
			if (jobs.size() >= max_jobs)
			{
				lua::lua_pushnil(s);
				return 1;
			}
			id = next_id++;
			jobs[id] = j;
		}
		std::thread(run, j, method, url, arg_string(s, 3), arg_string(s, 4), arg_string(s, 5)).detach();
		lua::lua_pushinteger(s, id);
		return 1;
	}

	int poll(lua::lua_State* s)
	{
		const int id = static_cast<int>(lua::lua_tonumber(s, 1));
		std::shared_ptr<job> j;
		{
			std::lock_guard lock(jobs_mutex);
			const auto it = jobs.find(id);
			if (it == jobs.end())
			{
				lua::lua_pushstring(s, "error");
				lua::lua_pushinteger(s, 0);
				lua::lua_pushstring(s, "unknown request");
				return 3;
			}
			j = it->second;
			if (j->state != 0)
				jobs.erase(it);
		}
		const int state = j->state;
		if (state == 0)
		{
			lua::lua_pushstring(s, "pending");
			lua::lua_pushinteger(s, 0);
			lua::lua_pushstring(s, "");
			return 3;
		}
		lua::lua_pushstring(s, state == 1 ? "done" : "error");
		lua::lua_pushinteger(s, j->status);
		lua::lua_pushlstring(s, j->body.data(), j->body.size());
		return 3;
	}

	class component final : public component_interface
	{
	public:
		void lua_start() override
		{
			const lua::luaL_Reg NetLibrary[] =
			{
				{"Start", start},
				{"Poll", poll},
				{nullptr, nullptr},
			};
			hks::hksI_openlib(game::UI_luaVM(), "Net", NetLibrary, 0, 1);
		}
	};
}

REGISTER_COMPONENT(net::component)
