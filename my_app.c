#include "my_app.h"

#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <Windows.h>
#include <stdio.h>
#include "resource.h"

#include "my_util.h"
#include "my_window.h"
#include "my_image.h"
#include "my_memory.h"
#include "my_thread.h"
#include "my_mutex.h"
#include "my_assembly.h"

int luaopen_lpeg(lua_State* L);

static lua_State* L;


#pragma pack(push, 1)
struct State {
	UINT8 keys[32];
	UINT32 time;
	UINT8 event;
	UINT8 arg1;
	UINT8 arg2;
	UINT8 arg3;
	UINT16 mousex;
	UINT16 mousey;
};
#pragma pack(pop)

struct State* sys = 0x10000;




lua_State* newvm() {

	lua_State* L = luaL_newstate();

	luaL_openlibs(L);

	luaopen_memory(L);
	lua_setglobal(L, "memory");

	luaopen_assembly(L);
	lua_setglobal(L, "assembly");

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

void draw();

#include <zydis/Zydis.h>

void testingthis() {
	printf("IN TESTING THIS!!\n");
}

void boot() {
	openConsole();

	// 4kb pages, 10mb reserved for user, starting at 0x10000
	void* mem = VirtualAlloc(0x10000, 0x10000000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	printf("testingthis = %p\n", testingthis);
	*((PUINT8)0x40000) = testingthis;

	L = newvm();

	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(IDR_MYTEXTFILE), MAKEINTRESOURCE(TEXTFILE));
	HGLOBAL rcData = LoadResource(handle, rc);
	DWORD size = SizeofResource(handle, rc);
	const char* data = LockResource(rcData);
	luaL_loadbuffer(L, data, size, "<boot>");
	lua_pcallk(L, 0, 0, 0, 0, NULL);

	int res = ((int(*)(int))0x40000)(213);
	printf("res = %d\n", res);

}

void mouseMoved(int x, int y) {
	lua_getglobal(L, "mousemove");
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	lua_pcall(L, 2, 0, 0);
}

void mouseDown(int b) {
	lua_getglobal(L, "mousedown");
	lua_pushinteger(L, b);
	lua_pcall(L, 1, 0, 0);

	useScreen(1 - screeni);
}

void mouseUp(int b) {
	lua_getglobal(L, "mouseup");
	lua_pushinteger(L, b);
	lua_pcall(L, 1, 0, 0);
}

void mouseWheel(int d) {
	lua_getglobal(L, "mousewheel");
	lua_pushinteger(L, d);
	lua_pcall(L, 1, 0, 0);
}

void keyDown(int vk) {
	lua_getglobal(L, "keydown");
	lua_pushinteger(L, vk);
	lua_pcall(L, 1, 0, 0);
}

void keyUp(int vk) {
	lua_getglobal(L, "keyup");
	lua_pushinteger(L, vk);
	lua_pcall(L, 1, 0, 0);
}

void keyChar(const char ch) {
	lua_getglobal(L, "keychar");
	lua_pushlstring(L, &ch, 1);
	lua_pcall(L, 1, 0, 0);
}
