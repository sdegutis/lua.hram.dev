#include "my_sync.h"

#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <Windows.h>

#include "my_app.h"

DWORD WINAPI StartThread(LPVOID param) {
	lua_State* L = param;
	lua_pcallk(L, lua_gettop(L) - 1, 1, 0, 0, 0);
	int res = lua_tointeger(L, -1);
	lua_close(L);
	return res;
}

static int sync_newthread(lua_State* L) {
	lua_State* L2 = newvm();

	size_t srclen;
	const char* src = luaL_checklstring(L, 1, &srclen);
	int ok = luaL_loadbuffer(L2, src, srclen, "<thread>");

	if (ok != LUA_OK) {
		lua_pushnil(L);
		lua_pushstring(L, lua_tostring(L2, -1));
		lua_rotate(L, 1, 2);
		lua_settop(L, 2);
		return 2;
	}

	for (auto i = 2; i <= lua_gettop(L); i++) {
		switch (lua_type(L, i)) {
		case LUA_TNIL:     lua_pushnil(L2); break;
		case LUA_TBOOLEAN: lua_pushboolean(L2, lua_toboolean(L, i)); break;
		case LUA_TSTRING:  lua_pushstring(L2, lua_tostring(L, i)); break;
		case LUA_TNUMBER:  lua_isinteger(L, i)
			? lua_pushinteger(L2, lua_tointeger(L, i))
			: lua_pushnumber(L2, lua_tonumber(L, i)); break;
		}
	}

	HANDLE t = CreateThread(NULL, 0, StartThread, L2, 0, NULL);
	lua_pushinteger(L, t);
	return 1;
}

static int sync_newcritsec(lua_State* L) {
	LPCRITICAL_SECTION cs = HeapAlloc(GetProcessHeap(), 0, sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(cs);
	lua_pushinteger(L, cs);
	return 1;
}

static int sync_delcritsec(lua_State* L) {
	LPCRITICAL_SECTION cs = luaL_checkinteger(L, -1);
	DeleteCriticalSection(cs);
	HeapFree(GetProcessHeap(), 0, cs);
	return 0;
}

static int sync_closehandle(lua_State* L) {
	HANDLE h = luaL_checkinteger(L, -1);
	CloseHandle(h);
}

static int sync_entercritsec(lua_State* L) {
	LPCRITICAL_SECTION cs = luaL_checkinteger(L, -1);
	EnterCriticalSection(cs);
}

static int sync_leavecritsec(lua_State* L) {
	LPCRITICAL_SECTION cs = luaL_checkinteger(L, -1);
	LeaveCriticalSection(cs);
}

static int sync_sleep(lua_State* L) {
	UINT64 ms = luaL_checkinteger(L, -1);
	Sleep(ms);
	return 0;
}

static int sync_newsemaphore(lua_State* L) {
	UINT64 init = luaL_checkinteger(L, 1);
	UINT64 max = luaL_checkinteger(L, 2);
	HANDLE sem = CreateSemaphore(NULL, init, max, NULL);
	lua_pushinteger(L, sem);
	return 1;
}

static const struct luaL_Reg threadlib[] = {
	{"newthread",    sync_newthread},
	{"closehandle",  sync_closehandle},
	{"newcritsec",   sync_newcritsec},
	{"delcritsec",   sync_delcritsec},
	{"entercritsec", sync_entercritsec},
	{"leavecritsec", sync_leavecritsec},
	{"newsemaphore", sync_newsemaphore},
	{"sleep",        sync_sleep},
	{NULL,NULL}
};

int luaopen_sync(lua_State* L) {
	luaL_newlib(L, threadlib);
	return 1;
}
