#include "my_app.h"

#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <Windows.h>
#include <stdio.h>
#include "resource.h"
#include <shlobj_core.h>
#include <KnownFolders.h>

#include "my_util.h"
#include "my_window.h"
#include "my_image.h"
#include "my_memory.h"
#include "my_sync.h"
#include "my_assembly.h"
#include "my_licenses.h"
#include "my_fontsheet.h"


void draw();
void toggleFullscreen();

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

int number_index(lua_State* L) {
	PUINT8 addr = lua_tointeger(L, 1);
	UINT64 offs = lua_tointeger(L, 2);
	lua_pushinteger(L, *(addr + offs));
	return 1;
}

int number_newindex(lua_State* L) {
	PUINT8 addr = lua_tointeger(L, 1);
	UINT64 offs = lua_tointeger(L, 2);
	UINT64 nval = lua_tointeger(L, 3);
	*(addr + offs) = nval;
	return 0;
}

int number_len(lua_State* L) {
	PUINT64 addr = lua_tointeger(L, 1);
	lua_pushinteger(L, *addr);
	return 1;
}

luaL_Reg numbermetamethods[] = {
	{"__len",      number_len},
	{"__call",     asm_exec},
	{"__index",    number_index},
	{"__newindex", number_newindex},
	{NULL,NULL}
};

int luaopen_lpeg(lua_State* L);

lua_State* newvm() {
	lua_State* L = luaL_newstate();

	luaL_openlibs(L);

	luaopen_memory(L);
	lua_setglobal(L, "memory");

	luaopen_asm(L);
	lua_setglobal(L, "asm");

	luaopen_image(L);
	lua_setglobal(L, "image");

	luaopen_sync(L);
	lua_setglobal(L, "sync");

	luaopen_lpeg(L);
	lua_setglobal(L, "lpeg");
	lua_settop(L, 0);

	luaL_newlibtable(L, numbermetamethods);
	luaL_setfuncs(L, numbermetamethods, 0);
	lua_pushinteger(L, 0);
	lua_pushvalue(L, -2);
	lua_setmetatable(L, -2);
	lua_pop(L, 2);

	return L;
}


int aplusbtimes2(int a, int b) {
	return (a + b) * 2;
}

int fullscreen(lua_State* L) {
	toggleFullscreen();
	return 0;
}

void boot() {
	void* mem = VirtualAlloc(0x10000, 0x100000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	CopyMemory(0x70000, third_party_licenses, sizeof(third_party_licenses));

	PUINT64 funcs = 0x10100;
	*funcs++ = screen->texture;
	*funcs++ = draw;
	*funcs++ = toggleFullscreen;
	*funcs++ = aplusbtimes2;
	*funcs++ = createImage(device, fontdata, 4 * 16, 6 * 6, 0);

	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(IDR_MYTEXTFILE), MAKEINTRESOURCE(TEXTFILE));
	HGLOBAL rcData = LoadResource(handle, rc);
	DWORD size = SizeofResource(handle, rc);
	const char* data = LockResource(rcData);

	L = newvm();

	lua_pushcfunction(L, fullscreen);
	lua_setglobal(L, "fullscreen");

	PWSTR wpath;
	SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &wpath);
	int wpathsize = WideCharToMultiByte(CP_UTF8, 0, wpath, -1, NULL, 0, NULL, NULL);
	PUINT8 ansipath = 0x20000;
	WideCharToMultiByte(CP_UTF8, 0, wpath, -1, ansipath, wpathsize, NULL, NULL);
	CoTaskMemFree(wpath);
	lua_pushstring(L, ansipath);
	lua_setglobal(L, "userdir");

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
	draw();
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
