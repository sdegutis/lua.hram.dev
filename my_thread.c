#include "my_thread.h"

#include <lua/lualib.h>
#include <lua/lauxlib.h>

static int thread_spawn(lua_State* L) {
	//auto s = luaL_checkstring(L, 1);




	//HANDLE heap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE | HEAP_GENERATE_EXCEPTIONS, 1000, 2000);
	//char* mem = (char*)HeapAlloc(heap, 0, 100);
	//int i = 0;
	//mem[i++] = 0x48;
	//mem[i++] = 0x89;
	//mem[i++] = 0xc8;
	//mem[i++] = 0x48;
	//mem[i++] = 0xff;
	//mem[i++] = 0xc0;
	//mem[i++] = 0xc3;
	//typedef int(*Func)(int);
	//Func fn = (Func)mem;
	//int res = fn(123);
	//printf("result = [%d]\n", res);




	//std::vector<std::variant<
	//	nullptr_t,
	//	bool,
	//	std::string,
	//	uint64_t,
	//	double>
	//> args;

	//for (auto i = 2; i <= lua_gettop(L); i++) {
	//	switch (lua_type(L, i)) {
	//	case LUA_TNIL:     args.push_back(nullptr); break;
	//	case LUA_TBOOLEAN: args.push_back((bool)lua_toboolean(L, i)); break;
	//	case LUA_TSTRING:  args.push_back(lua_tostring(L, i)); break;
	//	case LUA_TNUMBER:  args.push_back(lua_isinteger(L, i)
	//		? lua_tointeger(L, i)
	//		: lua_tonumber(L, i)); break;
	//	}
	//}

	//auto t = new std::jthread([s, args](std::stop_token stok) {
	//	auto L = app::newvm();

	//	auto err = luaL_loadstring(L, s);
	//	if (err != LUA_OK) {
	//		printf("thread.spawn err: %d\n", err);
	//		return;
	//	}

	//	for (auto& arg : args) {
	//		switch (arg.index()) {
	//		case 0: lua_pushnil(L); break;
	//		case 1: lua_pushboolean(L, std::get<bool>(arg)); break;
	//		case 2: lua_pushstring(L, std::get<std::string>(arg).c_str()); break;
	//		case 3: lua_pushinteger(L, std::get<uint64_t>(arg)); break;
	//		case 4: lua_pushnumber(L, std::get<double>(arg)); break;
	//		}
	//	}

	//	lua_pcallk(L, args.size(), 0, 0, 0, 0);
	//	});

	//lua_pushinteger(L, reinterpret_cast<uint64_t>(t));
	return 0;
}

static const luaL_Reg threadlib[] = {
	{"spawn", thread_spawn},
	{NULL,NULL}
};

int luaopen_thread(lua_State* L) {
	luaL_newlib(L, threadlib);
	return 1;
}
