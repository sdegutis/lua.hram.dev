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
	ZydisEncoderRequest req;
	ZeroMemory(&req, sizeof(req));

	req.mnemonic = ZYDIS_MNEMONIC_MOV;
	req.machine_mode = ZYDIS_MACHINE_MODE_LONG_64;
	req.operand_count = 2;
	req.operands[0].type = ZYDIS_OPERAND_TYPE_REGISTER;
	req.operands[0].reg.value = ZYDIS_REGISTER_RAX;
	req.operands[1].type = ZYDIS_OPERAND_TYPE_IMMEDIATE;
	req.operands[1].imm.u = 0x1337;

	ZyanU8 encoded_instruction[ZYDIS_MAX_INSTRUCTION_LENGTH];
	ZyanUSize encoded_length = sizeof(encoded_instruction);

	if (ZYAN_FAILED(ZydisEncoderEncodeInstruction(&req, encoded_instruction, &encoded_length))) {
		puts("Failed to encode instruction");
		return 1;
	}

	for (ZyanUSize i = 0; i < encoded_length; ++i) {
		printf("%02X ", encoded_instruction[i]);
	}


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

int luaopen_assembly(lua_State* L) {
	luaL_newlib(L, assemblylib);

	lua_createtable(L, ZYDIS_MNEMONIC_MAX_VALUE, 0);
	addops(L);
	lua_setfield(L, -2, "ops");

	lua_createtable(L, ZYDIS_REGISTER_MAX_VALUE, 0);
	addregs(L);
	lua_setfield(L, -2, "regs");

	return 1;
}
