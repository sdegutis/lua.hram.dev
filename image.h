#pragma once

#include <stdint.h>
#include <d3d11.h>
#include <lua/lua.hpp>

ID3D11Texture2D* createImage(ID3D11Device* device, void* data, int w, int h, int pw = 0);

int luaopen_image(lua_State* L);
