#include "my_memory.h"

#include <lua/lauxlib.h>

#include <Windows.h>
#include <heapapi.h>

static int memory_fill(lua_State* L) {
	void* mem = lua_tointeger(L, 1);
	lua_Integer val = lua_tointeger(L, 2);
	lua_Integer len = lua_tointeger(L, 3);
	memset(mem, val, len);
	return 0;
}

static int memory_tostr(lua_State* L) {
	const char* mem = lua_tointeger(L, 1);

	if (lua_type(L, 2) == LUA_TNUMBER) {
		UINT64 len = lua_tointeger(L, 2);
		lua_pushlstring(L, mem, len);
	}
	else {
		lua_pushstring(L, mem);
	}

	return 1;
}

static int memory_copy(lua_State* L) {
	void* dst = lua_tointeger(L, 1);
	void* src;
	SIZE_T size;

	if (lua_type(L, 2) == LUA_TSTRING) {
		src = lua_tolstring(L, 2, &size);
	}
	else {
		src = lua_tointeger(L, 2);
		size = lua_tointeger(L, 3);
	}
	CopyMemory(dst, src, size);

	return 0;
}

static const luaL_Reg memorylib[] = {
	{"memset",  memory_fill},
	{"memcpy",  memory_copy},
	{"strndup", memory_tostr},
	{NULL,NULL}
};

int luaopen_memory(lua_State* L) {
	luaL_setfuncs(L, memorylib, 0);
	return 1;
}
