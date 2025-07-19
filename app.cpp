#include "app.h"


#include "util.h"
#include "window.h"
#include "screen.h"
#include "image.h"
#include "memory.h"
#include "thread.h"
#include "mutex.h"

extern "C" int luaopen_lpeg(lua_State* L);

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


lua_State* app::newvm()
{
	auto L = luaL_newstate();

	luaL_openlibs(L);

	luaopen_memory(L);
	lua_setglobal(L, "memory");

	luaopen_image(L);
	lua_setglobal(L, "image");

	luaopen_thread(L);
	lua_setglobal(L, "thread");

	luaopen_lpeg(L);
	lua_setglobal(L, "lpeg");

	luaopen_mutex(L);
	lua_setglobal(L, "mutex");

	return L;
}

void app::boot()
{
	openConsole();

	//printf("s = %d\n", sizeof(Bit));
	//printf("s = %d\n", sizeof(State));

	mvm = newvm();


	luaL_dostring(mvm, R"(

		-- foo

		print(lpeg)

		print(pcall(function()

		print('in foo!')

		m = mutex.create()

		t = thread.create([[
			local m = ...
			mutex.lock(m)
			print('locked in thread')
			mutex.unlock(m)
			print('done in thread')
		]], m)

		--thread.sleep(1000)
		mutex.lock(m)
		print('locked in main')
		mutex.unlock(m)
		print('done in main')


		-- print(t)

		-- require 'bar'



		local m = memory.malloc(3*3*4)
		for i = 0,3*3*4-1 do memory.set(m+i, 8, math.random(0xff)-1) end
		local img = image.create(m, 3, 3)
		--memory.free(m)

		image.copy(nil, img, 0, 0)

		local m2 = memory.malloc(9*9*4)
		memory.fill(m2, 0, 9*9*4)
		local img2 = image.create(m2, 9, 9)
		memory.free(m2)

		image.copy(img2, img, 0, 0, 0, 0, 3, 3)
		image.copy(img2, img, 6, 0, 0, 0, 3, 3)
		image.copy(img2, img, 0, 6, 0, 0, 3, 3)
		image.copy(img2, img, 6, 6, 0, 0, 3, 3)

		image.delete(img)



		function mousemove(x, y)
			--image.copy(nil, img2, x, y)
			image.update(nil, x, y, 2, 2, m+4*4, 3*4)
			print("mouse moved", x, y)
		end

		function mousewheel(d)
			print("mouse wheel", d)
		end

		function mouseup(b)
			print("mouse up", b)
		end

		function mousedown(b)
			print("mouse up", b)
		end

		function keyup(k)
			print("key up", k)
		end

		function keydown(k)
			print("key up", k)
		end

		function keychar(s)
			print("key char", s)
		end

		end))

	)");
}

void app::mouseMoved(int x, int y) {
	lua_getglobal(mvm, "mousemove");
	lua_pushinteger(mvm, x);
	lua_pushinteger(mvm, y);
	lua_pcall(mvm, 2, 0, 0);
}

void app::mouseDown(int b) {
	lua_getglobal(mvm, "mousedown");
	lua_pushinteger(mvm, b);
	lua_pcall(mvm, 1, 0, 0);

	useScreen(1 - screeni);
}

void app::mouseUp(int b) {
	lua_getglobal(mvm, "mouseup");
	lua_pushinteger(mvm, b);
	lua_pcall(mvm, 1, 0, 0);
}

void app::mouseWheel(int d) {
	lua_getglobal(mvm, "mousewheel");
	lua_pushinteger(mvm, d);
	lua_pcall(mvm, 1, 0, 0);
}

void app::keyDown(int vk) {
	lua_getglobal(mvm, "keydown");
	lua_pushinteger(mvm, vk);
	lua_pcall(mvm, 1, 0, 0);
}

void app::keyUp(int vk) {
	lua_getglobal(mvm, "keyup");
	lua_pushinteger(mvm, vk);
	lua_pcall(mvm, 1, 0, 0);
}

void app::keyChar(const char ch) {
	lua_getglobal(mvm, "keychar");
	lua_pushlstring(mvm, &ch, 1);
	lua_pcall(mvm, 1, 0, 0);
}
