#pragma once

#include <d3d11.h>
#include <lua/lua.h>

ID3D11Texture2D* createImage(ID3D11Device* device, void* data, UINT w, UINT h, UINT pw);

int luaopen_image(lua_State* L);
