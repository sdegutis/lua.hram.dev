#include "my_assembly.h"

#include <Windows.h>

#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include <zydis/Zydis.h>

static int assembly_exec(lua_State* L) {
	typedef UINT64(*F)();
	F mem = lua_tointeger(L, 1);
	UINT64 arg = lua_tointeger(L, 2);
	UINT64 res = mem(arg);
	lua_pushinteger(L, res);
	return 1;
}

static int assembly_assemble(lua_State* L) {
	PUINT8 dst = luaL_checkinteger(L, 1);
	UINT64 opcode = luaL_checkinteger(L, 2);

	ZydisEncoderRequest req;
	ZeroMemory(&req, sizeof(req));

	req.mnemonic = opcode;
	req.machine_mode = ZYDIS_MACHINE_MODE_LONG_64;
	req.operand_count = lua_gettop(L) - 2;

	for (int i = 0; i < req.operand_count; i++) {
		lua_rawgeti(L, 3 + i, 1);
		req.operands[i].type = lua_tointeger(L, -1);
		lua_pop(L, 1);

		switch (req.operands[i].type) {

		case ZYDIS_OPERAND_TYPE_REGISTER:
			lua_rawgeti(L, 3 + i, 2);
			req.operands[i].reg.value = lua_tointeger(L, -1);
			lua_rawgeti(L, 3 + i, 3);
			req.operands[i].reg.is4 = lua_toboolean(L, -1);
			lua_pop(L, 2);
			break;

		case ZYDIS_OPERAND_TYPE_POINTER:
			lua_rawgeti(L, 3 + i, 2);
			req.operands[i].ptr.segment = lua_tointeger(L, -1);
			lua_rawgeti(L, 3 + i, 3);
			req.operands[i].ptr.offset = lua_tointeger(L, -1);
			lua_pop(L, 2);
			break;

		case ZYDIS_OPERAND_TYPE_MEMORY:
			lua_rawgeti(L, 3 + i, 2);
			req.operands[i].mem.base = lua_tointeger(L, -1);
			lua_rawgeti(L, 3 + i, 3);
			req.operands[i].mem.index = lua_tointeger(L, -1);
			lua_rawgeti(L, 3 + i, 4);
			req.operands[i].mem.scale = lua_tointeger(L, -1);
			lua_rawgeti(L, 3 + i, 5);
			req.operands[i].mem.displacement = lua_tointeger(L, -1);
			lua_rawgeti(L, 3 + i, 6);
			req.operands[i].mem.size = lua_tointeger(L, -1);
			lua_pop(L, 5);
			break;

		case ZYDIS_OPERAND_TYPE_IMMEDIATE:
			lua_rawgeti(L, 3 + i, 3);
			BOOL s = lua_tointeger(L, -1) == -1;
			lua_rawgeti(L, 3 + i, 2);
			if (s) req.operands[i].imm.s = lua_tointeger(L, -1);
			else   req.operands[i].imm.u = lua_tointeger(L, -1);
			lua_pop(L, 2);
			break;
		}
	}

	ZyanUSize encoded_length;
	if (ZYAN_FAILED(ZydisEncoderEncodeInstruction(&req, dst, &encoded_length))) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushinteger(L, encoded_length);
	return 1;
}

static int assembly_disassemble(lua_State* L) {
	UINT64 addr = luaL_checknumber(L, 1);
	UINT64 size = luaL_checknumber(L, 2);

	char buffer[256];
	luaL_Buffer b;
	luaL_buffinit(L, &b);

	ZyanUSize offset = 0;
	ZydisDisassembledInstruction instruction;
	while (ZYAN_SUCCESS(ZydisDisassembleIntel(
		ZYDIS_MACHINE_MODE_LONG_64,
		addr,
		addr + offset,
		size - offset,
		&instruction
	))) {
		sprintf_s(buffer, 256, "%016llX  %s\n", addr, instruction.text);
		luaL_addstring(&b, buffer);
		offset += instruction.info.length;
		addr += instruction.info.length;
	}

	luaL_pushresult(&b);
	return 1;
}

static const luaL_Reg assemblylib[] = {
	{"exec",        assembly_exec},
	{"assemble",    assembly_assemble},
	{"disassemble", assembly_disassemble},
	{NULL,NULL}
};

void addops(lua_State* L);
void addregs(lua_State* L);
void addtypes(lua_State* L);

int luaopen_assembly(lua_State* L) {
	luaL_newlib(L, assemblylib);

	lua_createtable(L, ZYDIS_MNEMONIC_MAX_VALUE, 0);
	addops(L);
	lua_setfield(L, -2, "ops");

	lua_createtable(L, ZYDIS_REGISTER_MAX_VALUE, 0);
	addregs(L);
	lua_setfield(L, -2, "regs");

	lua_createtable(L, ZYDIS_OPERAND_TYPE_MAX_VALUE, 0);
	addtypes(L);
	lua_setfield(L, -2, "types");

	return 1;
}
