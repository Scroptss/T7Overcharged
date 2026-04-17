#pragma once
#include "game/game.hpp"
#define WEAK __declspec(selectany)

#define LUA_REGISTRYINDEX	(-10000)
#define LUA_ENVIRONINDEX	(-10001)
#define LUA_GLOBALSINDEX	(-10002)
#define OFFSET(address) (uintptr_t)((address - 0x140000000) + (uintptr_t)GetModuleHandle(NULL))
namespace lua
{
	WEAK game::symbol<void(lua_State* s, int narg, const char* extramsg)> luaL_argerror{ OFFSET(0x141D40A80) }; //updated
	WEAK game::symbol<int (lua_State* s, long nargs, long nresults)> lua_pcall{ OFFSET(0x141D479E0) }; //updated
	WEAK game::symbol<void(lua_State* s, int index, const char* k)> lua_setfield{ OFFSET(0x1414296A0) }; //updated
	WEAK game::symbol<const void*  (lua_State* s, int index)> lua_topointer{ OFFSET(0x141D42BC0) }; //updated
	WEAK game::symbol<int (lua_State* s, int index)> lua_toboolean{ OFFSET(0x1414373F0) };
	WEAK game::symbol<hksUint64 (lua_State* s, int index)> lua_toui64{ OFFSET(0x141D404D0) }; //updated
	WEAK game::symbol<void (lua_State* s, const char* str, size_t l)> lua_pushlstring{ OFFSET(0x140A18430) }; //unchanged
	WEAK game::symbol<const char* (lua_State* s, int idx, const char* fname, int szhint)> luaL_findtable{ OFFSET(0x141D46C80) }; //updated
	WEAK game::symbol<const char* (lua_State* s, const char* str)> lua_pushstring{ OFFSET(0x140A186B0) }; //unchanged
	WEAK game::symbol<int (lua_State* s, const char* fmt, ...)> luaL_error{ OFFSET(0x141D46BF0) }; //updated

	WEAK game::symbol<int(lua_State* s, int index)> lua_isnumber{ OFFSET(0x141429370) }; //updated

	//guessing this is not needed since we never use it
	//WEAK game::symbol<__int64 (const char* key, const char* value, lua_State* luaVM)> Lua_SetTableString{ (uintptr_t)GetModuleHandle(NULL) + 0x32534688 }; 

	void luaL_register(lua_State* s, const char* libname, const luaL_Reg* l);
	void lua_setglobal(lua_State* s, const char* k);
	void lua_pop(lua_State* s, int n);
	HksNumber lua_tonumber(lua_State* s, int index);
	const char* lua_tostring(lua_State* s, int index);
	void lua_pushnumber(lua_State* s, HksNumber n);
	void lua_pushinteger(lua_State* s, int n);
	void lua_pushnil(lua_State* s);
	void lua_pushboolean(lua_State* s, int b);
	void lua_pushvalue(lua_State* s, int index);
	void lua_pushfstring(lua_State* s, const char* fmt, ...);
	void lua_pushvfstring(lua_State* s, const char* fmt, va_list* argp);
	void lua_getfield(lua_State* s, int index, const char* k);
	void lua_getglobal(lua_State* s, const char* k);
}


