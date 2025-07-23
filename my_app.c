#include "my_app.h"

#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <Windows.h>
#include <stdio.h>
#include "resource.h"
#include <shlobj_core.h>
#include <KnownFolders.h>

#include "my_window.h"
#include "my_memory.h"
#include "my_sync.h"
#include "my_assembly.h"
#include "my_licenses.h"
#include "my_fontsheet.h"


void draw();
void toggleFullscreen();

static lua_State* L;

struct AppState {
	UINT8 appversion;
	UINT8 eventid;
	UINT16 eventarg;
	UINT8 inflags;
	UINT8 keymods;
	UINT8 reserved2;
	UINT8 reserved3;
	UINT32 time;
	UINT16 mousex;
	UINT16 mousey;
	UINT8 keys[32];
	UINT64 addrs[26];
	UINT8 screen[128 * 72];
	UINT8 font[16 * 4 * 4 * 6];
	CHAR licenses[1280];
};

struct AppState* sys = 0x30000;


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

	lua_pushglobaltable(L);
	luaopen_memory(L);

	luaopen_asm(L);
	lua_setglobal(L, "asm");

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

static void initfont();

void boot() {

	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

	//CHAR szFileName[MAX_PATH];
	//GetModuleFileNameA(NULL, szFileName, MAX_PATH);
	//char* bare = strrchr(szFileName, '\\') + 1;

	sys->appversion = 200;

	memcpy(sys->licenses, third_party_licenses, sizeof(third_party_licenses));
	initfont();

	int funcs = 0;
	sys->addrs[funcs++] = aplusbtimes2;
	sys->addrs[funcs++] = toggleFullscreen;

	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(IDR_MYTEXTFILE), MAKEINTRESOURCE(TEXTFILE));
	HGLOBAL rcData = LoadResource(handle, rc);
	DWORD size = SizeofResource(handle, rc);
	const char* data = LockResource(rcData);

	L = newvm();

	PWSTR wpath;
	PUINT8 ansipath = 0x33000;
	SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &wpath);
	WideCharToMultiByte(CP_UTF8, 0, wpath, -1, ansipath, MAX_PATH, NULL, NULL);
	CoTaskMemFree(wpath);
	lua_pushstring(L, ansipath);
	lua_setglobal(L, "userdir");

	luaL_loadbuffer(L, data, size, "<boot>");
	lua_pcallk(L, 0, 0, 0, 0, NULL);
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

void callsig(enum asmevent ev, UINT32 arg) {
	sys->eventid = ev;
	sys->eventarg = arg;
	lua_getglobal(L, "sig");
	lua_call(L, 0, 0);
}

void tick(DWORD delta, DWORD now) {
	sys->time = now;
	callsig(asmevent_tick, delta);

	if (sys->inflags) {
		sys->inflags = 0;
		devicecontext->lpVtbl->UpdateSubresource(devicecontext, screen.texture, 0, NULL, &sys->screen, 128, 0);
		draw();
	}
}

void mouseMoved(int x, int y) {
	sys->mousex = x;
	sys->mousey = y;
	callsig(asmevent_mousemove, ((x & 0xffff) << 16) | (y & 0xffff));
}

void mouseDown(int b) {
	callsig(asmevent_mousedown, b);
}

void mouseUp(int b) {
	callsig(asmevent_mouseup, b);
}

void mouseWheel(int d) {
	callsig(asmevent_mousewheel, d);
}

void keyDown(int vk) {
	callsig(asmevent_keydown, vk);
}

void keyUp(int vk) {
	callsig(asmevent_keyup, vk);
}

void syskeyDown(int vk) {
	callsig(asmevent_keydown, vk);
}

void syskeyUp(int vk) {
	callsig(asmevent_keyup, vk);
}

void keyChar(const char ch) {
	callsig(asmevent_keychar, ch);
}

void sysChar(const char ch) {
	callsig(asmevent_keychar, ch);
}



#define FW (4)
#define FH (6)
#define SW (16)
#define SH (6)

static void initfont() {
	PUINT8 ptr = sys->font;
	int z = 0;
	for (int sy = 0; sy < SH; sy++) {
		for (int sx = 0; sx < SW; sx++) {
			for (int fy = 0; fy < FH; fy++) {
				for (int fx = 0; fx < FW; fx++) {
					int i = (fy * SW * FW) + (sy * SH * SW * FW) + (sx * FW) + fx;
					*ptr++ = fontdata[i] ? 0x0a : 0x30;
				}
			}
		}
	}
}
