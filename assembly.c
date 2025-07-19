#include "assembly.h"

#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include <Windows.h>

static int assembly_exec(lua_State* L) {
	typedef UINT64(*F)();
	F mem = lua_tointeger(L, 1);
	UINT64 arg = lua_tointeger(L, 2);
	UINT64 res = mem(arg);
	lua_pushinteger(L, res);
	return 1;
}

static const luaL_Reg assemblylib[] = {
	{"exec", assembly_exec},
	{NULL,NULL}
};

int luaopen_assembly(lua_State* L) {
	luaL_newlib(L, assemblylib);
	return 1;
}
