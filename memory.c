//#include "memory.h"
//
//#include <cstdint>
//#include <memory>
//
//static int newmemory(lua_State* L) {
//	auto len = luaL_checkinteger(L, 1);
//	auto mem = new uint8_t[len];
//	lua_pushinteger(L, reinterpret_cast<uint64_t>(mem));
//	return 1;
//}
//
//static int delmemory(lua_State* L) {
//	auto mem = reinterpret_cast<uint8_t*>(lua_tointeger(L, 1));
//	delete[] mem;
//	return 0;
//}
//
//static int filmemory(lua_State* L) {
//	auto mem = reinterpret_cast<uint8_t*>(lua_tointeger(L, 1));
//	auto val = lua_tointeger(L, 2);
//	auto len = lua_tointeger(L, 3);
//	memset(mem, val, len);
//	return 0;
//}
//
//static int getmemory(lua_State* L) {
//	auto mem = lua_tointeger(L, 1);
//	auto siz = lua_tointeger(L, 2);
//
//	int good = 0;
//	lua_Integer val = 0;
//	switch (siz) {
//	case 8:   val = *reinterpret_cast<uint8_t*>(mem);  good = 1; break;
//	case 16:  val = *reinterpret_cast<uint16_t*>(mem); good = 1; break;
//	case 32:  val = *reinterpret_cast<uint32_t*>(mem); good = 1; break;
//	case 64:  val = *reinterpret_cast<uint64_t*>(mem); good = 1; break;
//	case -8:  val = *reinterpret_cast<int8_t*>(mem);   good = 1; break;
//	case -16: val = *reinterpret_cast<int16_t*>(mem);  good = 1; break;
//	case -32: val = *reinterpret_cast<int32_t*>(mem);  good = 1; break;
//	case -64: val = *reinterpret_cast<int64_t*>(mem);  good = 1; break;
//	}
//
//	if (good)
//		lua_pushinteger(L, val);
//	else
//		lua_pushnil(L);
//
//	return 1;
//}
//
//static int setmemory(lua_State* L) {
//	auto mem = lua_tointeger(L, 1);
//	auto siz = lua_tointeger(L, 2);
//	auto val = lua_tointeger(L, 3);
//
//	int good = 0;
//	lua_Integer set = 0;
//	switch (siz) {
//	case 8:   set = *reinterpret_cast<uint8_t*>(mem) = val;  good = 1; break;
//	case 16:  set = *reinterpret_cast<uint16_t*>(mem) = val; good = 1; break;
//	case 32:  set = *reinterpret_cast<uint32_t*>(mem) = val; good = 1; break;
//	case 64:  set = *reinterpret_cast<uint64_t*>(mem) = val; good = 1; break;
//	case -8:  set = *reinterpret_cast<int8_t*>(mem) = val;   good = 1; break;
//	case -16: set = *reinterpret_cast<int16_t*>(mem) = val;  good = 1; break;
//	case -32: set = *reinterpret_cast<int32_t*>(mem) = val;  good = 1; break;
//	case -64: set = *reinterpret_cast<int64_t*>(mem) = val;  good = 1; break;
//	}
//
//	if (good)
//		lua_pushinteger(L, set);
//	else
//		lua_pushnil(L);
//
//	return 1;
//}
//
//static const luaL_Reg memorylib[] = {
//	{"malloc", newmemory},
//	{"free",   delmemory},
//	{"get",    getmemory},
//	{"set",    setmemory},
//	{"fill",   filmemory},
//	{NULL,NULL}
//};
//
//int luaopen_memory(lua_State* L) {
//	luaL_newlib(L, memorylib);
//	return 1;
//}
