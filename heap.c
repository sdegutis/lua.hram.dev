#include "heap.h"

#include <Windows.h>
#include <heapapi.h>
#include <lua/lauxlib.h>

static int luawrap_heap_create(lua_State* L) {
	lua_Integer len = luaL_checkinteger(L, 1);
	lua_Integer max = luaL_checkinteger(L, 2);
	int exe = lua_toboolean(L, 3);
	void* heap = HeapCreate(exe ? HEAP_CREATE_ENABLE_EXECUTE : 0, len, max);
	lua_pushinteger(L, heap);
	return 1;
}

static const luaL_Reg heaplib[] = {
	{"create", luawrap_heap_create},
	{NULL,NULL}
};

int luaopen_heap(lua_State* L) {
	luaL_newlib(L, heaplib);
	return 1;
}
