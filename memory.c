#include "memory.h"

#include <stdint.h>
#include <lua/lauxlib.h>

#include <Windows.h>
#include <heapapi.h>

static int luawrap_memory_alloc(lua_State* L) {
	lua_Integer len = luaL_checkinteger(L, 1);
	void* mem = HeapAlloc(GetProcessHeap(), 0, len);
	lua_pushinteger(L, mem);
	return 1;
}

static int luawrap_memory_free(lua_State* L) {
	void* mem = lua_tointeger(L, 1);
	HeapFree(GetProcessHeap(), 0, mem);
	return 0;
}

static int luawrap_memory_fill(lua_State* L) {
	void* mem = lua_tointeger(L, 1);
	lua_Integer val = lua_tointeger(L, 2);
	lua_Integer len = lua_tointeger(L, 3);
	memset(mem, val, len);
	return 0;
}

static int luawrap_memory_get(lua_State* L) {
	lua_Integer mem = lua_tointeger(L, 1);
	lua_Integer siz = lua_tointeger(L, 2);

	int good = 0;
	lua_Integer val = 0;
	switch (siz) {
	case 8:   val = *(uint8_t*)mem;  good = 1; break;
	case 16:  val = *(uint16_t*)mem; good = 1; break;
	case 32:  val = *(uint32_t*)mem; good = 1; break;
	case 64:  val = *(uint64_t*)mem; good = 1; break;
	case -8:  val = *(int8_t*)mem;   good = 1; break;
	case -16: val = *(int16_t*)mem;  good = 1; break;
	case -32: val = *(int32_t*)mem;  good = 1; break;
	case -64: val = *(int64_t*)mem;  good = 1; break;
	}

	if (good)
		lua_pushinteger(L, val);
	else
		lua_pushnil(L);

	return 1;
}

static int luawrap_memory_set(lua_State* L) {
	void* mem = lua_tointeger(L, 1);
	lua_Integer siz = lua_tointeger(L, 2);
	lua_Integer val = lua_tointeger(L, 3);

	int good = 0;
	lua_Integer set = 0;
	switch (siz) {
	case 8:   set = (*((uint8_t*)mem) = val);  good = 1; break;
	case 16:  set = (*((uint16_t*)mem) = val); good = 1; break;
	case 32:  set = (*((uint32_t*)mem) = val); good = 1; break;
	case 64:  set = (*((uint64_t*)mem) = val); good = 1; break;
	case -8:  set = (*((int8_t*)mem) = val);   good = 1; break;
	case -16: set = (*((int16_t*)mem) = val);  good = 1; break;
	case -32: set = (*((int32_t*)mem) = val);  good = 1; break;
	case -64: set = (*((int64_t*)mem) = val);  good = 1; break;
	}

	if (good)
		lua_pushinteger(L, set);
	else
		lua_pushnil(L);

	return 1;
}

static const luaL_Reg memorylib[] = {
	{"malloc", luawrap_memory_alloc},
	{"free",   luawrap_memory_free},
	{"get",    luawrap_memory_get},
	{"set",    luawrap_memory_set},
	{"fill",   luawrap_memory_fill},
	{NULL,NULL}
};

int luaopen_memory(lua_State* L) {
	luaL_newlib(L, memorylib);
	return 1;
}
