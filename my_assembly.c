#include "my_assembly.h"

#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include <zydis/Zydis.h>
#include <inttypes.h>

#include <Windows.h>

static int assembly_exec(lua_State* L) {
	typedef UINT64(*F)();
	F mem = lua_tointeger(L, 1);
	UINT64 arg = lua_tointeger(L, 2);
	UINT64 res = mem(arg);
	lua_pushinteger(L, res);
	return 1;
}

static int assembly_disassemble(lua_State* L) {

	UINT64 runtime_address = luaL_checknumber(L, 1);
	UINT64 size = luaL_checknumber(L, 2);

	char buffer[256];

	luaL_Buffer b;
	luaL_buffinit(L, &b);

	ZyanU8* data = runtime_address;
	ZyanUSize offset = 0;
	ZydisDisassembledInstruction instruction;
	while (ZYAN_SUCCESS(ZydisDisassembleIntel(
		/* machine_mode:    */ ZYDIS_MACHINE_MODE_LONG_64,
		/* runtime_address: */ runtime_address,
		/* buffer:          */ data + offset,
		/* length:          */ size - offset,
		/* instruction:     */ &instruction
	))) {
		sprintf_s(buffer, 256, "%016llX  %s\n", runtime_address, instruction.text);
		luaL_addstring(&b, buffer);
		offset += instruction.info.length;
		runtime_address += instruction.info.length;
	}

	luaL_pushresult(&b);
	return 1;
}

static const luaL_Reg assemblylib[] = {
	{"exec", assembly_exec},
	{"disassemble", assembly_disassemble},
	{NULL,NULL}
};

int luaopen_assembly(lua_State* L) {
	luaL_newlib(L, assemblylib);
	return 1;
}
