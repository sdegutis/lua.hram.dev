#include "app.h"


#include "util.h"
#include "window.h"
#include "screen.h"
#include "image.h"
#include "memory.h"
#include "thread.h"
#include "mutex.h"

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

	printf("testing\n");

	luaL_dostring(mvm,
		"print(lpeg)\n"
	);

	//*
	luaL_dostring(mvm,

		"		-- foo\n"
		"\n"
		"		print(lpeg)\n"
		"\n"
		"		print(pcall(function()\n"
		"\n"
		"		print('in foo!')\n"
		"\n"
		"		--[=[\n"
		"		m = mutex.create()\n"
		"\n"
		"		t = thread.create([[\n"
		"			local m = ...\n"
		"			mutex.lock(m)\n"
		"			print('locked in thread')\n"
		"			mutex.unlock(m)\n"
		"			print('done in thread')\n"
		"		]], m)\n"
		"\n"
		"		--thread.sleep(1000)\n"
		"		mutex.lock(m)\n"
		"		print('locked in main')\n"
		"		mutex.unlock(m)\n"
		"		print('done in main')\n"
		"\n"
		"\n"
		"		-- print(t)\n"
		"\n"
		"		-- require 'bar'\n"
		"\n"
		"		--]=]\n"
		"\n"
		"\n"
		"\n"
		"		local m = memory.malloc(3*3*4)\n"
		"		for i = 0,3*3*4-1 do memory.set(m+i, 8, math.random(0xff)-1) end\n"
		"		local img = image.create(m, 3, 3)\n"
		"		--memory.free(m)\n"
		"\n"
		"		image.copy(nil, img, 0, 0)\n"
		"\n"
		"		local m2 = memory.malloc(9*9*4)\n"
		"		memory.fill(m2, 0, 9*9*4)\n"
		"		local img2 = image.create(m2, 9, 9)\n"
		"		memory.free(m2)\n"
		"\n"
		"		image.copy(img2, img, 0, 0, 0, 0, 3, 3)\n"
		"		image.copy(img2, img, 6, 0, 0, 0, 3, 3)\n"
		"		image.copy(img2, img, 0, 6, 0, 0, 3, 3)\n"
		"		image.copy(img2, img, 6, 6, 0, 0, 3, 3)\n"
		"\n"
		"		image.delete(img)\n"
		"\n"
		"\n"
		"\n"
		"		function mousemove(x, y)\n"
		"			--image.copy(nil, img2, x, y)\n"
		"			image.update(nil, x, y, 2, 2, m+4*4, 3*4)\n"
		"			print('mouse moved', x, y)\n"
		"		end\n"
		"\n"
		"		function mousewheel(d)\n"
		"			print('mouse wheel', d)\n"
		"		end\n"
		"\n"
		"		function mouseup(b)\n"
		"			print('mouse up', b)\n"
		"		end\n"
		"\n"
		"		function mousedown(b)\n"
		"			print('mouse up', b)\n"
		"		end\n"
		"\n"
		"		function keyup(k)\n"
		"			print('key up', k)\n"
		"		end\n"
		"\n"
		"		function keydown(k)\n"
		"			print('key up', k)\n"
		"		end\n"
		"\n"
		"		function keychar(s)\n"
		"			print('key char', s)\n"
		"		end\n"
		"\n"
		"		end))\n"

	);
	//*/
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
