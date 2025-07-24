#include "my_assembly.h"

#include <Windows.h>

#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include <zydis/Zydis.h>

int asm_exec(lua_State* L) {
	typedef UINT64(*F)();
	F mem = lua_tointeger(L, 1);
	UINT64 arg = lua_tointeger(L, 2);
	UINT64 res = mem(arg);
	lua_pushinteger(L, res);
	return 1;
}

static int asm_assemble(lua_State* L) {
	PUINT8 dst = luaL_checkinteger(L, 1);
	UINT64 opcode = luaL_checkinteger(L, 2);

	ZydisEncoderRequest req;
	ZeroMemory(&req, sizeof(req));

	int count = 0;
	while (lua_type(L, count + 3) == LUA_TTABLE) count++;

	req.mnemonic = opcode;
	req.machine_mode = ZYDIS_MACHINE_MODE_LONG_64;
	req.operand_count = count;

	for (int i = 0; i < count; i++) {
		lua_rawgeti(L, 3 + i, 1);
		req.operands[i].type = lua_tointeger(L, -1);
		lua_pop(L, 1);

		switch (req.operands[i].type) {

		case ZYDIS_OPERAND_TYPE_REGISTER:
			lua_rawgeti(L, 3 + i, 2);      req.operands[i].reg.value = lua_tointeger(L, -1);
			lua_getfield(L, 3 + i, "is4"); req.operands[i].reg.is4 = lua_toboolean(L, -1);
			lua_pop(L, 2);
			break;

		case ZYDIS_OPERAND_TYPE_POINTER:
			lua_getfield(L, 3 + i, "segment"); req.operands[i].ptr.segment = lua_tointeger(L, -1);
			lua_getfield(L, 3 + i, "offset");  req.operands[i].ptr.offset = lua_tointeger(L, -1);
			lua_pop(L, 2);
			break;

		case ZYDIS_OPERAND_TYPE_MEMORY:
			lua_getfield(L, 3 + i, "base");  req.operands[i].mem.base = lua_tointeger(L, -1);
			lua_getfield(L, 3 + i, "index"); req.operands[i].mem.index = lua_tointeger(L, -1);
			lua_getfield(L, 3 + i, "scale"); req.operands[i].mem.scale = lua_tointeger(L, -1);
			lua_getfield(L, 3 + i, "disp");  req.operands[i].mem.displacement = lua_tointeger(L, -1);
			lua_getfield(L, 3 + i, "bytes");  req.operands[i].mem.size = lua_tointeger(L, -1);
			lua_pop(L, 5);
			break;

		case ZYDIS_OPERAND_TYPE_IMMEDIATE:
			lua_getfield(L, 3 + i, "signed");
			BOOL s = lua_toboolean(L, -1);
			lua_rawgeti(L, 3 + i, 2);
			if (s) req.operands[i].imm.s = lua_tointeger(L, -1);
			else   req.operands[i].imm.u = lua_tointeger(L, -1);
			lua_pop(L, 2);
			break;
		}
	}

	lua_rotate(L, 1, -(count + 2));
	lua_pop(L, count + 2);

	ZyanUSize encoded_length;
	ZyanStatus status = ZydisEncoderEncodeInstruction(&req, dst, &encoded_length);
	if (ZYAN_FAILED(status)) {
		switch (status) {
		case ZYDIS_STATUS_INSTRUCTION_TOO_LONG:
			lua_pushliteral(L, "error on line [1]: instruction too long");
			break;
		case ZYDIS_STATUS_IMPOSSIBLE_INSTRUCTION:
			lua_pushliteral(L, "error on line [1]: impossible instruction");
			break;
		case ZYAN_STATUS_INVALID_ARGUMENT:
			lua_pushliteral(L, "error on line [1]: invalid argument");
			break;
		case ZYAN_STATUS_INSUFFICIENT_BUFFER_SIZE:
			lua_pushliteral(L, "error on line [1]: insufficient buffer size");
			break;
		}
	}
	else {
		lua_pushinteger(L, dst + encoded_length);
	}

	return 1;
}

static int asm_assembleall(lua_State* L) {
	int line = 1;
	while (1) {
		asm_assemble(L);

		if (lua_type(L, -1) == LUA_TSTRING) {
			const char* err = lua_tostring(L, -1);

			lua_pushnil(L);

			char linestr[256];
			memset(linestr, 0, 256);
			sprintf_s(linestr, 256, "%d", line);

			luaL_Buffer b;
			luaL_buffinit(L, &b);
			luaL_addgsub(&b, err, "1", linestr);
			luaL_pushresult(&b);

			return 2;
		}

		if (lua_gettop(L) == 1) {
			return 1;
		}

		lua_rotate(L, 1, 1);
		line++;
	}
}

static int asm_disassemble(lua_State* L) {
	UINT64 addr = luaL_checknumber(L, 1);
	UINT64 size = luaL_checknumber(L, 2);
	PUINT8 data = addr;

	char buffer[256];
	luaL_Buffer b;
	luaL_buffinit(L, &b);

	ZyanUSize offset = 0;
	ZydisDisassembledInstruction instruction;

	while (1) {
		ZyanStatus status = ZydisDisassembleIntel(
			ZYDIS_MACHINE_MODE_LONG_64,
			addr,
			data + offset,
			size - offset,
			&instruction
		);
		if (!ZYAN_SUCCESS(status)) {
			//printf("failed: %d\n", status == ZYDIS_STATUS_NO_MORE_DATA);
			break;
		}

		sprintf_s(buffer, 256, "%llX  %s\n", addr, instruction.text);
		luaL_addstring(&b, buffer);
		offset += instruction.info.length;
		addr += instruction.info.length;
	}

	luaL_pushresult(&b);
	return 1;
}

static const luaL_Reg asmlib[] = {
	{"exec", asm_exec},
	{"dasm", asm_disassemble},
	{"asm",  asm_assembleall},
	{NULL,NULL}
};

void addops(lua_State* L);
void addregs(lua_State* L);
void addtypes(lua_State* L);

int luaopen_asm(lua_State* L) {
	luaL_newlib(L, asmlib);

	lua_createtable(L, ZYDIS_MNEMONIC_MAX_VALUE, 0);
	addops(L);
	lua_setfield(L, -2, "ops");

	lua_createtable(L, ZYDIS_REGISTER_MAX_VALUE, 0);
	addregs(L);
	lua_setfield(L, -2, "reg");

	lua_createtable(L, ZYDIS_OPERAND_TYPE_MAX_VALUE, 0);
	addtypes(L);
	lua_setfield(L, -2, "loc");

	return 1;
}
