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
void blitimmediately();

static lua_State* L;

struct AppState {
	UINT16 appversion;
	UINT8 eventid;
	UINT8 eventarg;
	UINT8 inflags;
	UINT8 keymods;
	UINT8 cursorcol;
	UINT8 cursorrow;
	UINT32 time;
	UINT8 mousex;
	UINT8 mousey;
	UINT16 reserved1;
	UINT8 keys[32];
	UINT64 addrs[26];
	UINT8 screen[128 * 72];
	UINT8 font[16 * 4 * 6 * 6];
	CHAR reserved2[512];
	PUINT8 userdata;
};

struct AppState* sys = 0x30000;


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

	//AllocConsole();
	//freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	//freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

	CHAR szFileName[MAX_PATH];
	GetModuleFileNameA(NULL, szFileName, MAX_PATH);
	char* exename = strrchr(szFileName, '\\') + 1;
	BOOL skipwelcome = strchr(exename, '0') != NULL;

	sys->appversion = 221;

	memcpy(&sys->userdata, third_party_licenses, sizeof(third_party_licenses));
	initfont();

	int funcs = 0;
	sys->addrs[funcs++] = aplusbtimes2;
	sys->addrs[funcs++] = toggleFullscreen;
	sys->addrs[funcs++] = blitimmediately;

	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(IDR_MYTEXTFILE), MAKEINTRESOURCE(TEXTFILE));
	HGLOBAL rcData = LoadResource(handle, rc);
	DWORD size = SizeofResource(handle, rc);
	const char* data = LockResource(rcData);

	L = newvm();

	PWSTR wpath;
	UINT8 ansipath[MAX_PATH];
	SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &wpath);
	WideCharToMultiByte(CP_UTF8, 0, wpath, -1, ansipath, MAX_PATH, NULL, NULL);
	CoTaskMemFree(wpath);
	lua_pushstring(L, ansipath);
	lua_setglobal(L, "userdir");

	lua_pushboolean(L, skipwelcome);
	lua_setglobal(L, "skipwelcome");

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

void blitimmediately() {
	devicecontext->lpVtbl->UpdateSubresource(devicecontext, screen.texture, 0, NULL, &sys->screen, 128, 0);
	draw();
}

void tick(DWORD delta, DWORD now) {
	sys->time = now;
	callsig(asmevent_tick, delta);

	if (sys->inflags & 1) {
		blitimmediately();
	}

	sys->inflags = 0;
}

void mouseMoved(int x, int y) {
	callsig(asmevent_mousemove, 7);
	sys->mousex = x;
	sys->mousey = y;
}

void togglekeystate(int vk, int down) {
	UINT8 bit = 0;
	if /**/ (vk == VK_CONTROL) bit = 1;
	else if (vk == VK_MENU)    bit = 2;
	else if (vk == VK_SHIFT)   bit = 3;

	if (bit) {
		bit--;
		sys->keymods = sys->keymods & ~(1 << bit) | down << bit;
	}

	UINT8 byteindex = vk / 8;
	bit = vk % 8;
	sys->keys[byteindex] = (sys->keys[byteindex] & ~(1 << bit)) | (down << bit);
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
	togglekeystate(vk, 1);
	callsig(asmevent_keydown, vk);
}

void keyUp(int vk) {
	togglekeystate(vk, 0);
	callsig(asmevent_keyup, vk);
}

void syskeyDown(int vk) {
	togglekeystate(vk, 1);
	callsig(asmevent_keydown, vk);
}

void syskeyUp(int vk) {
	togglekeystate(vk, 0);
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
					*ptr++ = fontdata[i] ? 0xf0 : 0;
				}
			}
		}
	}
}
