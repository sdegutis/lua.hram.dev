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
	union {
		struct {
			UINT16 arg1;
			UINT16 arg2;
		} args;
		UINT32 arg;
	};
	UINT32 time;
} *sys = 0x10000;


int asm_exec(lua_State* L);

luaL_Reg numbermetamethods[] = {
	{"__call", asm_exec},
	{NULL,NULL}
};

lua_State* newvm() {

	lua_State* L = luaL_newstate();

	luaL_openlibs(L);

	luaopen_memory(L);
	lua_setglobal(L, "memory");

	luaopen_asm(L);
	lua_setglobal(L, "asm");

	luaopen_image(L);
	lua_setglobal(L, "image");

	//luaopen_thread(L);
	//lua_setglobal(L, "thread");

	luaopen_lpeg(L);
	lua_setglobal(L, "lpeg");

	//luaopen_mutex(L);
	//lua_setglobal(L, "mutex");

	luaL_newlibtable(L, numbermetamethods);
	luaL_setfuncs(L, numbermetamethods, 0);
	lua_pushinteger(L, 0);
	lua_pushvalue(L, -2);
	lua_setmetatable(L, -2);
	lua_pop(L, 2);

	return L;
}


void testingthis(int a) {
	int b = 0;
	printf("Return address from %s: %p\n", __FUNCTION__, _ReturnAddress());
	draw();
	printf("IN TESTING THIS a!! a=[%d] b=[%d]\n", a++, b++);
	printf("IN TESTING THIS b!! a=[%d] b=[%d]\n", a++, b++);
	printf("IN TESTING THIS c!! a=[%d] b=[%d]\n", a++, b++);
	printf("BLIT!\n");
	draw();
	return a + b * 2;
}

void boot() {
	openConsole();

	void* mem = VirtualAlloc(0x10000, 0x100000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	CopyMemory(0x70000, third_party_licenses, sizeof(third_party_licenses));

	PUINT64 funcs = 0x60000;
	*funcs++ = draw;
	*funcs++ = testingthis;

	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(IDR_MYTEXTFILE), MAKEINTRESOURCE(TEXTFILE));
	HGLOBAL rcData = LoadResource(handle, rc);
	DWORD size = SizeofResource(handle, rc);
	const char* data = LockResource(rcData);

	L = newvm();
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
	sys->arg = delta;
	sys->time = now;
	callint();
}

void mouseMoved(int x, int y) {
	sys->event = asmevent_mousemove;
	sys->args.arg1 = x;
	sys->args.arg2 = y;
	callint();
}

void mouseDown(int b) {
	sys->event = asmevent_mousedown;
	sys->arg = b;
	callint();
}

void mouseUp(int b) {
	sys->event = asmevent_mouseup;
	sys->arg = b;
	callint();
}

void mouseWheel(int d) {
	sys->event = asmevent_mousewheel;
	sys->arg = d;
	callint();
}

void keyDown(int vk) {
	sys->event = asmevent_keydown;
	sys->arg = vk;
	callint();
}

void keyUp(int vk) {
	sys->event = asmevent_keyup;
	sys->arg = vk;
	callint();
}

void keyChar(const char ch) {
	sys->event = asmevent_keychar;
	sys->arg = ch;
	callint();
}
