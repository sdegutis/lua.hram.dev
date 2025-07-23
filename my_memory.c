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

static int memory_tostrn(lua_State* L) {
	const char* mem = lua_tointeger(L, 1);
	UINT64 len = lua_tointeger(L, 2);
	lua_pushlstring(L, mem, len);
	return 1;
}

static int memory_tostr(lua_State* L) {
	const char* mem = lua_tointeger(L, 1);
	lua_pushstring(L, mem);
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
	{"strndup", memory_tostrn},
	{"strdup",  memory_tostr},
	{NULL,NULL}
};

int number_index(lua_State* L) {
	PUINT8 addr = lua_tointeger(L, 1);
	UINT64 offs = lua_tointeger(L, 2);
	lua_pushinteger(L, *(addr + offs));
	return 1;
}

int number_newindex(lua_State* L) {
	PUINT8 addr = lua_tointeger(L, 1);
	UINT64 offs = lua_tointeger(L, 2);
	UINT64 nval = lua_tointeger(L, 3);
	*(addr + offs) = nval;
	return 0;
}

int number_len(lua_State* L) {
	PUINT64 addr = lua_tointeger(L, 1);
	lua_pushinteger(L, *addr);
	return 1;
}

int asm_exec(lua_State* L);

luaL_Reg numbermetamethods[] = {
	{"__len",      number_len},
	{"__call",     asm_exec},
	{"__index",    number_index},
	{"__newindex", number_newindex},
	{NULL,NULL}
};

int luaopen_memory(lua_State* L) {
	luaL_newlibtable(L, numbermetamethods);
	luaL_setfuncs(L, numbermetamethods, 0);
	lua_pushinteger(L, 0);
	lua_pushvalue(L, -2);
	lua_setmetatable(L, -2);
	lua_pop(L, 2);

	luaL_setfuncs(L, memorylib, 0);
	return 1;
}
