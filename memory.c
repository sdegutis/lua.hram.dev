#include "memory.h"

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

static int memory_get(lua_State* L) {
	lua_Integer mem = lua_tointeger(L, 1);
	lua_Integer siz = lua_tointeger(L, 2);

	int good = 0;
	lua_Integer val = 0;
	switch (siz) {
	case 8:   val = *(PUINT8)mem;  good = 1; break;
	case 16:  val = *(PUINT16)mem; good = 1; break;
	case 32:  val = *(PUINT32)mem; good = 1; break;
	case 64:  val = *(PUINT64)mem; good = 1; break;
	case -8:  val = *(PINT8)mem;   good = 1; break;
	case -16: val = *(PINT16)mem;  good = 1; break;
	case -32: val = *(PINT32)mem;  good = 1; break;
	case -64: val = *(PINT64)mem;  good = 1; break;
	}

	if (good)
		lua_pushinteger(L, val);
	else
		lua_pushnil(L);

	return 1;
}

static int memory_set(lua_State* L) {
	void* mem = lua_tointeger(L, 1);
	lua_Integer siz = lua_tointeger(L, 2);
	lua_Integer val = lua_tointeger(L, 3);

	int good = 0;
	lua_Integer set = 0;
	switch (siz) {
	case 8:   set = (*((PUINT8)mem) = val);  good = 1; break;
	case 16:  set = (*((PUINT16)mem) = val); good = 1; break;
	case 32:  set = (*((PUINT32)mem) = val); good = 1; break;
	case 64:  set = (*((PUINT64)mem) = val); good = 1; break;
	case -8:  set = (*((PINT8)mem) = val);   good = 1; break;
	case -16: set = (*((PINT16)mem) = val);  good = 1; break;
	case -32: set = (*((PINT32)mem) = val);  good = 1; break;
	case -64: set = (*((PINT64)mem) = val);  good = 1; break;
	}

	if (good)
		lua_pushinteger(L, set);
	else
		lua_pushnil(L);

	return 1;
}

static const luaL_Reg memorylib[] = {
	{"get",  memory_get},
	{"set",  memory_set},
	{"fill", memory_fill},
	{"copy", memory_copy},
	{NULL,NULL}
};

int luaopen_memory(lua_State* L) {
	luaL_newlib(L, memorylib);
	return 1;
}
