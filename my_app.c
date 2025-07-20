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
#include "licenses.h"

int luaopen_lpeg(lua_State* L);
void draw();

static lua_State* L;
int intref;

struct {
	UINT32 event;
	UINT16 arg1;
	UINT16 arg2;
	UINT32 time;
} *sys = 0x10000;


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


void testingthis(int a, int b) {
	//draw();
	//draw();
	//draw();
	//printf("IN TESTING THIS a!! a=[%d] b=[%d]\n", a++, b++);
	//printf("IN TESTING THIS b!! a=[%d] b=[%d]\n", a++, b++);
	//printf("IN TESTING THIS c!! a=[%d] b=[%d]\n", a++, b++);
}


void blit() {
	//draw();
	printf("BLIT!\n");
}

void boot() {
	openConsole();

	void* mem = VirtualAlloc(0x10000, 0x100000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	CopyMemory(0x70000, third_party_licenses, sizeof(third_party_licenses));
	*((PUINT64)0x60000) = blit;
	*((PUINT64)0x30000) = testingthis;

	// setup lua
	L = newvm();
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(IDR_MYTEXTFILE), MAKEINTRESOURCE(TEXTFILE));
	HGLOBAL rcData = LoadResource(handle, rc);
	DWORD size = SizeofResource(handle, rc);
	const char* data = LockResource(rcData);
	luaL_loadbuffer(L, data, size, "<boot>");
	lua_pcallk(L, 0, 0, 0, 0, NULL);
	lua_getglobal(L, "int");
	intref = luaL_ref(L, LUA_REGISTRYINDEX);
}

enum asmevent {
	asmevent_tick,
	asmevent_mousemove,
	asmevent_mousewheel,
	asmevent_mousedown,
	asmevent_mouseup,
	asmevent_keydown,
	asmevent_keyup,
	asmevent_keychar,
};

void callint() {
	lua_rawgeti(L, LUA_REGISTRYINDEX, intref);
	lua_call(L, 0, 0);
}

void tick(DWORD delta, DWORD now) {
	sys->event = asmevent_tick;
	sys->arg1 = delta;
	sys->time = now;
	callint();
	//draw();
}

void mouseMoved(int x, int y) {
	sys->event = asmevent_mousemove;
	sys->arg1 = x;
	sys->arg2 = y;
	callint();
}

void mouseDown(int b) {
	sys->event = asmevent_mousedown;
	sys->arg1 = b;
	callint();
}

void mouseUp(int b) {
	sys->event = asmevent_mouseup;
	sys->arg1 = b;
	callint();
}

void mouseWheel(int d) {
	sys->event = asmevent_mousewheel;
	sys->arg1 = d;
	callint();
}

void keyDown(int vk) {
	sys->event = asmevent_keydown;
	sys->arg1 = vk;
	callint();
}

void keyUp(int vk) {
	sys->event = asmevent_keyup;
	sys->arg1 = vk;
	callint();
}

void keyChar(const char ch) {
	sys->event = asmevent_keychar;
	sys->arg1 = ch;
	callint();
}
