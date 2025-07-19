#pragma once

#include <lua/lua.hpp>

namespace app {

	lua_State* newvm();

	void boot();

	void mouseMoved(int x, int y);
	void mouseDown(int b);
	void mouseUp(int b);
	void mouseWheel(int d);

	void keyDown(int vk);
	void keyUp(int vk);
	void keyChar(const char ch);

}
