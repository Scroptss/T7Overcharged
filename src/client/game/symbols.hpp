#pragma once

#define WEAK __declspec(selectany)

namespace game
{
#define REBASE(address) (uintptr_t)((address - 0x140000000) + game::base)
#define OFFSET(address) (uintptr_t)((address - 0x140000000) + (uintptr_t)GetModuleHandle(NULL))

	/* BG */
	WEAK symbol<int(void* ps, void* weapon/*const playerState_t* ps, const Weapon weapon*/)> BG_GetAmmoInClip{ OFFSET(0x14266DCA0) }; 
	WEAK symbol<int(void* ps, void* weapon/*const playerState_t* ps, const Weapon weapon*/)> BG_GetTotalAmmoReserve{ OFFSET(0x14266E440) }; 
	WEAK symbol<int(void* weapon/*const Weapon weapon*/)> BG_GetClipSize{ OFFSET(0x14266DF60) }; 
	WEAK symbol<bool(void* weapon/*const Weapon weapon*/)> BG_IsLeftHandWeapon{ OFFSET(0x14267B420) }; 
	WEAK symbol<bool(void* weapon/*const Weapon weapon*/)> BG_IsDualWield{ OFFSET(0x14267B1B0) }; 
	WEAK symbol<void* (void* weapon/*const Weapon weapon*/)> BG_GetDualWieldWeapon{ OFFSET(0x142676960) }; //updated

	/* BUILT-IN */
	WEAK symbol<const char* (const char* s0, const char* substr)> I_stristr{ OFFSET(0x14227CCE0) }; //updated
	WEAK symbol<__int64(int* a1, __int64 a2, __int64 a3)> PLmemset{ OFFSET(0x142BC53B0) }; //updated
	WEAK symbol<int(char* string, unsigned int count, const char* format, char* ap)> vsnprintf{ OFFSET(0x142BC44C0) }; //updated

	/* CG */
	WEAK symbol<void(void* obj, const game::XAnimNotifyInfo* notifyInfo, void* info, const unsigned int notifyFilter, bool shutdown, bool skipNonImportantNotifies)> CG_ProcessClientNote{ OFFSET(0x140255C70) }; //updated

	/* CMD */
	// Variables
	WEAK symbol<game::cmd_function_t*> cmd_functions{ OFFSET(0x15681EFD8) }; //updated 

	/* COM */
	WEAK symbol<char(const char* file, int line, lua::ErrorCode code, const char* fmt, ...)> Com_Error_{ OFFSET(0x1420EC0B0) }; //updated
	WEAK symbol<const char* (const char* name)> Com_GameInfo_GetGameTypeRefCaps{ OFFSET(0x1420E7570) }; //updated
	WEAK symbol<const char* (const char* name)> Com_GameInfo_GetGameTypeRef{ OFFSET(0x1420E74F0) }; //updated
	WEAK symbol<const char* (const char* name)> Com_GameInfo_GetMapRef{ OFFSET(0x1420E7760) }; //updated
	WEAK symbol<bool()> Com_IsRunningUILevel{ OFFSET(0x1420EFFB0) }; //updated
	WEAK symbol<bool(consoleChannel_e channel, consoleLabel_e label, const char* fmt, ...)> Com_Printf{ OFFSET(0x1420F0B60) }; //updated
	WEAK symbol<bool(consoleChannel_e channel, consoleLabel_e label, const char* msg, int error)> Com_PrintMessage{ OFFSET(0x1420F0800) }; //updated
	WEAK symbol<bool(eGameModes mode)> Com_SessionMode_IsGameMode{ OFFSET(0x1420EB270) }; //updated
	WEAK symbol<bool(eModes mode)> Com_SessionMode_IsMode{ OFFSET(0x1420EB2B0) }; //updated
	WEAK symbol<int(char* dest, size_t size, const char* fmt, ...)> Com_sprintf{ OFFSET(0x142BC3FB0) }; //updated

	/* CSC/GSC */
	WEAK symbol<ScrString_t(const char* s)> GScr_AllocString{ OFFSET(0x141A77150) }; //updated
	WEAK symbol<int(scriptInstance_t inst, unsigned int index)> Scr_GetInt{ OFFSET(0x1412EB810) }; //updated
	WEAK symbol<const char* (scriptInstance_t inst, unsigned int index)> Scr_GetString{ OFFSET(0x1412EBAC0) }; //updated
	// Variables
	WEAK symbol<BuiltinFunctionDef> isProfileBuildFunctionDef{ OFFSET(0x143258D70) }; //updated

	/* DB */
	WEAK symbol<int(XAssetType type)> DB_GetXAssetTypeSize{ OFFSET(0x1413E9E00) }; //updated 0x13E9DE0
	// Variables
	static XAssetPool* DB_XAssetPool = reinterpret_cast<XAssetPool*>(OFFSET(0x1493883F0)); //updated
	static bool s_luaInFrontend = ((bool)OFFSET(0x143394BE8)); //updated
	static lua::lua_State* UI_luaVM = ((lua::lua_State*)((*(INT64*)OFFSET(0x159BF7E08)))); //updated

	/* DObj */
	WEAK symbol<int(void*/*const DObj*/ obj, ScrString_t name, BoneIndex* index, int modelNum)> DObjGetBoneIndex{ OFFSET(0x1422C4900) }; //updated
	WEAK symbol<LocalClientNum_t(void*/*const DObj*/ obj)> DObjGetLocalClientIndex{ OFFSET(0x1422BE3C0) }; //updated
	WEAK symbol<int(void*/*const DObj*/ obj, int* partBits, uint16_t boneIndex, const vec3_t* trans, const vec3_t* angles)> DObjSetLocalTag{ OFFSET(0x1422C5770) }; //updated

	/* DVAR */
	WEAK symbol<game::dvarStrHash_t(const char* str)> Dvar_GenerateHash{ OFFSET(0x14133DC10) }; //updated
	WEAK symbol<bool(const dvar_t* dvar)> Dvar_GetBool{ OFFSET(0x142260A70) }; //updated
	WEAK symbol<const char* (const dvar_t* dvar)> Dvar_GetString{ OFFSET(0x142263130) }; //updated
	WEAK symbol<void(const char* dvarName, const char* string, bool createIfMissing)> Dvar_SetFromStringByName{ OFFSET(0x14226B0A0) }; //updated
	// Variables
	WEAK symbol<char*> s_dvarPool{ OFFSET(0x157A47320) }; //updated
	WEAK symbol<int> g_dvarCount{ OFFSET(0x157A472CC) }; //updated

	/* LOBBYSESSION */
	WEAK symbol<int(INT64 lobbySession, LobbyClientType clientType)> LobbySession_GetClientCount{ OFFSET(0x141EBEC60) }; //updated

	/* LUA / UI */
	WEAK symbol<const char* (const char* name)> UI_SafeTranslateString{ OFFSET(0x1422328F0) }; //updated
	WEAK symbol<game::LUIElement* (const char* rootName, lua::lua_State* luaVM)> UI_GetRootElement{ OFFSET(0x14268C050) }; //updated
	WEAK symbol<void(LUIScopedEvent* event, lua::lua_State* luaVM, const char* rootName, const char* eventName)> GetLUIScopedEvent{ OFFSET(0x142685CE0) }; //updated
	WEAK symbol<void(LUIScopedEvent* event)> ExecuteLUIScopedEvent{ OFFSET(0x142685F10) }; //updated

	/* MATERIAL */
	WEAK symbol<INT64(const char* name, int imageTrack, bool errorIfMissing, int waitTime)> Material_RegisterHandle{ OFFSET(0x141CC87C0) }; //updated

	/* SYS */
	WEAK symbol<int()> Sys_IsDatabaseReady2{ OFFSET(0x14212B680) }; //updated
	WEAK symbol<void()> Sys_ShowConsole{ OFFSET(0x1422BA910) }; //updated
	WEAK symbol<bool()> Sys_IsMainThread{ OFFSET(0x14212B6B0) }; //updated
}
