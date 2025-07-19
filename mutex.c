//#include "mutex.h"
//
//#include <mutex>
//
//static int newmutex(lua_State* L) {
//	auto m = new std::mutex();
//	lua_pushinteger(L, reinterpret_cast<uint64_t>(m));
//	return 1;
//}
//
//static int delmutex(lua_State* L) {
//	auto t = reinterpret_cast<std::mutex*>(luaL_checkinteger(L, 1));
//	delete t;
//	return 0;
//}
//
//static int lokmutex(lua_State* L) {
//	auto t = reinterpret_cast<std::mutex*>(luaL_checkinteger(L, 1));
//	t->lock();
//	return 0;
//}
//
//static int unlmutex(lua_State* L) {
//	auto t = reinterpret_cast<std::mutex*>(luaL_checkinteger(L, 1));
//	t->unlock();
//	return 0;
//}
//
//static int trymutex(lua_State* L) {
//	auto t = reinterpret_cast<std::mutex*>(luaL_checkinteger(L, 1));
//	auto ok = t->try_lock();
//	lua_pushboolean(L, ok);
//	return 1;
//}
//
//static const luaL_Reg mutexlib[] = {
//	{"create", newmutex},
//	{"delete", delmutex},
//	{"lock",   lokmutex},
//	{"unlock", unlmutex},
//	{"try",    trymutex},
//	{NULL,NULL}
//};
//
//int luaopen_mutex(lua_State* L) {
//	luaL_newlib(L, mutexlib);
//	return 1;
//}
