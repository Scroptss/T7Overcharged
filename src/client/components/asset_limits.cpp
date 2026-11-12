#include <std_include.hpp>
#include "asset_limits.hpp"
#include "game/game.hpp"
#include "loader/component_loader.hpp"
#include "havok/hks_api.hpp"
#include "havok/lua_api.hpp"

namespace asset_limits
{
	void resize(game::XAssetType type, int newSize)
	{
		auto structSize = game::DB_GetXAssetTypeSize(type);
		auto assetPool = &game::DB_XAssetPool[type];
		// TODO: Support resizing assetPools to be less than current size
		if (assetPool != 0 && assetPool->itemCount < newSize)
		{
			auto newBlock = (game::AssetLink*)calloc(newSize - assetPool->itemCount, structSize);
			if (newBlock != 0)
			{
				game::AssetLink* blockPtr = (game::AssetLink*)assetPool->pool;
				game::AssetLink* nextBlockPtr = newBlock;
				auto size_c = newSize - 1;
				do
				{
					if (!blockPtr->next)
					{
						blockPtr->next = nextBlockPtr;
						blockPtr = nextBlockPtr;
						nextBlockPtr = (game::AssetLink*)((uint8_t*)nextBlockPtr + structSize);
					}
					else
					{
						blockPtr = (game::AssetLink*)((uint8_t*)blockPtr + structSize);
					}
					--size_c;
				} while (size_c);
				assetPool->itemCount = newSize;
			}
		}
	}

	// AssetLimits.Info( assetType ) -> pool size, entries in use (XAssetType index, e.g. 4 xmodel,
	// 6 material, 9 image). Read-only: shows how close a map gets to a limit.
	int info(lua::lua_State* s)
	{
		const int type = static_cast<int>(lua::lua_tonumber(s, 1));
		if (type < 0 || type >= game::ASSET_TYPE_COUNT)
		{
			lua::lua_pushnil(s);
			return 1;
		}
		const auto* pool = &game::DB_XAssetPool[type];
		const int size = pool->itemCount;
		int free_count = 0;
		for (auto* link = pool->freeHead; link && free_count < size; link = link->next)
			free_count++;
		lua::lua_pushinteger(s, size);
		lua::lua_pushinteger(s, size - free_count);
		return 2;
	}

	class component final : public component_interface
	{
	public:
		void lua_start() override
		{
			const lua::luaL_Reg AssetLimitsLibrary[] =
			{
				{"Info", info},
				{nullptr, nullptr},
			};
			hks::hksI_openlib(game::UI_luaVM(), "AssetLimits", AssetLimitsLibrary, 0, 1);
		}
	};
}

REGISTER_COMPONENT(asset_limits::component)
