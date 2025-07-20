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
	printf("thread done\n");
	return res;
}

static int sync_spawn(lua_State* L) {
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

static int sync_close(lua_State* L) {
	CloseHandle(lua_tointeger(L, -1));
	printf("closed\n");
	return 0;
}

static const struct luaL_Reg threadlib[] = {
	{"newthread", sync_spawn},
	{"close", sync_close},
	{NULL,NULL}
};

int luaopen_sync(lua_State* L) {
	luaL_newlib(L, threadlib);
	return 1;
}
