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
	UINT32 event;
	UINT16 arg1;
	UINT16 arg2;
	UINT32 time;
	UINT16 mousex;
	UINT16 mousey;
	UINT8 keys[32];
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


void __stdcall testingthis(int a, int b) {
	printf("IN TESTING THIS a!! a=[%d] b=[%d]\n", a++, b++);
	printf("IN TESTING THIS b!! a=[%d] b=[%d]\n", a++, b++);
	printf("IN TESTING THIS c!! a=[%d] b=[%d]\n", a++, b++);
}

const char third_party_licenses[] =
"License of Lua, LPeg, and Zydis (all MIT) is as follows.\n"
"\n"
"Lua   : Copyright (c) 1994–2025 Lua.org, PUC-Rio.\n"
"LPeg  : Copyright (c) 2007-2023 Lua.org, PUC-Rio.\n"
"Zydis : Copyright (c) 2014-2024 Florian Bernd\n"
"Zydis : Copyright (c) 2014-2024 Joel Höner\n"
"\n"
"Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n"
"\n"
"The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n"
"\n"
"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n"
;

int intref;

void boot() {
	openConsole();

	// 4kb pages, 1mb reserved for user, starting at 0x10000
	void* mem = VirtualAlloc(0x10000, 0x100000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// add license
	CopyMemory(0x70000, third_party_licenses, sizeof(third_party_licenses));

	// testing
	*((PUINT64)0x40000) = testingthis;
	printf("testingthis = %p\n", testingthis);
	printf("testingthis = %p\n", *(PUINT64)0x40000);

	L = newvm();

	// run <boot>
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(IDR_MYTEXTFILE), MAKEINTRESOURCE(TEXTFILE));
	HGLOBAL rcData = LoadResource(handle, rc);
	DWORD size = SizeofResource(handle, rc);
	const char* data = LockResource(rcData);
	luaL_loadbuffer(L, data, size, "<boot>");
	lua_pcallk(L, 0, 0, 0, 0, NULL);

	lua_getglobal(L, "int");
	intref = luaL_ref(L, LUA_REGISTRYINDEX);

	//int res = ((int(*)(int))0x50000)(213);
	//printf("res = %d\n", res);

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

void tick(DWORD delta) {
	sys->event = asmevent_tick;
	sys->arg1 = delta;
	callint();

	//lua_getglobal(L, "tick");
	//lua_pcall(L, 0, 0, 0);

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
	//useScreen(1 - screeni);
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
