#pragma once

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

lua_State* newvm();

void boot();

void mouseMoved(int x, int y);
void mouseDown(int b);
void mouseUp(int b);
void mouseWheel(int d);

void keyDown(int vk);
void keyUp(int vk);
void keyChar(const char ch);
