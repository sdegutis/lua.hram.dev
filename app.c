#include "app.h"


#include "util.h"
#include "window.h"
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


#define MAXMEM = 0x0000700000001000

lua_State* newvm()
{



	SYSTEM_INFO info;
	GetSystemInfo(&info);
	printf("page size = %x\n", info.dwPageSize);
	printf("page size = %p\n", info.lpMinimumApplicationAddress);
	printf("page size = %p\n", info.lpMaximumApplicationAddress);
	//printf("page size = %p\n", info.boun);

	printf("%d\n", 0x10000);

	// 4kb pages, 10mb reserved for user
	void* mem = VirtualAlloc(0x10000, 0x10000000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	printf("reserved = %p\n", mem);

	int i = 0;
	*(PUINT)(0x10000 + i++) = 0x48;
	*(PUINT)(0x10000 + i++) = 0x89;
	*(PUINT)(0x10000 + i++) = 0xc8;
	*(PUINT)(0x10000 + i++) = 0x48;
	*(PUINT)(0x10000 + i++) = 0xff;
	*(PUINT)(0x10000 + i++) = 0xc0;
	*(PUINT)(0x10000 + i++) = 0xc3;

	typedef int(*F)(int);

	F f = 0x10000;

	int res = f(213);
	printf("res = %d\n", res);



	//0000000000010000
	//00007FFFFFFEFFFF
	//0000700000001000

	//HeapCreate(HEAP_CREATE_ENABLE_EXECUTE| HEAP_NO_SERIALIZE, )

	lua_State* L = luaL_newstate();

	luaL_openlibs(L);

	luaopen_memory(L);
	lua_setglobal(L, "memory");

	luaopen_heap(L);
	lua_setglobal(L, "heap");

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
