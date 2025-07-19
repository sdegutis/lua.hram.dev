#include "app.h"


#include "util.h"
#include "window.h"
#include "screen.h"
#include "image.h"
#include "memory.h"
#include "thread.h"
#include "mutex.h"

#include <Windows.h>
#include <stdio.h>
#include "resource.h"

int luaopen_lpeg(lua_State* L);

static lua_State* mvm;


//#pragma pack(push, 1)
//
//struct Bit {
//	uint8_t bit : 1;
//};
//
//struct State {
//	uint8_t mousex;
//	uint8_t mousey;
//	Bit bits[256];
//	uint8_t mousez;
//};
//
//#pragma pack(pop)


lua_State* newvm()
{
	lua_State* L = luaL_newstate();

	luaL_openlibs(L);

	luaopen_memory(L);
	lua_setglobal(L, "memory");

	luaopen_image(L);
	lua_setglobal(L, "image");

	//luaopen_thread(L);
	//lua_setglobal(L, "thread");

	luaopen_lpeg(L);
	lua_setglobal(L, "lpeg");

	//luaopen_mutex(L);
	//lua_setglobal(L, "mutex");

	return L;
}

void boot()
{
	openConsole();

	//printf("s = %d\n", sizeof(Bit));
	//printf("s = %d\n", sizeof(State));

	mvm = newvm();


	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(IDR_MYTEXTFILE), MAKEINTRESOURCE(TEXTFILE));
	HGLOBAL rcData = LoadResource(handle, rc);
	DWORD size = SizeofResource(handle, rc);
	const char* data = LockResource(rcData);

	luaL_loadbuffer(mvm, data, size, "boot");
	lua_pcallk(mvm, 0, 0, 0, 0, NULL);
}

void mouseMoved(int x, int y) {
	lua_getglobal(mvm, "mousemove");
	lua_pushinteger(mvm, x);
	lua_pushinteger(mvm, y);
	lua_pcall(mvm, 2, 0, 0);
}

void mouseDown(int b) {
	lua_getglobal(mvm, "mousedown");
	lua_pushinteger(mvm, b);
	lua_pcall(mvm, 1, 0, 0);

	useScreen(1 - screeni);
}

void mouseUp(int b) {
	lua_getglobal(mvm, "mouseup");
	lua_pushinteger(mvm, b);
	lua_pcall(mvm, 1, 0, 0);
}

void mouseWheel(int d) {
	lua_getglobal(mvm, "mousewheel");
	lua_pushinteger(mvm, d);
	lua_pcall(mvm, 1, 0, 0);
}

void keyDown(int vk) {
	lua_getglobal(mvm, "keydown");
	lua_pushinteger(mvm, vk);
	lua_pcall(mvm, 1, 0, 0);
}

void keyUp(int vk) {
	lua_getglobal(mvm, "keyup");
	lua_pushinteger(mvm, vk);
	lua_pcall(mvm, 1, 0, 0);
}

void keyChar(const char ch) {
	lua_getglobal(mvm, "keychar");
	lua_pushlstring(mvm, &ch, 1);
	lua_pcall(mvm, 1, 0, 0);
}
