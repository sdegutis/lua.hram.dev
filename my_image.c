#include "my_image.h"

#include <lua/lauxlib.h>

#include "my_util.h"
#include "my_window.h"

ID3D11Texture2D* createImage(ID3D11Device* device, void* data, UINT w, UINT h, UINT pw) {
	if (pw == 0) pw = w * 4;

	ID3D11Texture2D* texture = NULL;

	D3D11_TEXTURE2D_DESC texturedesc;
	ZeroMemory(&texturedesc, sizeof(texturedesc));
	texturedesc.Width = w;
	texturedesc.Height = h;
	texturedesc.MipLevels = 1;
	texturedesc.ArraySize = 1;
	texturedesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texturedesc.SampleDesc.Count = 1;
	texturedesc.Usage = D3D11_USAGE_DEFAULT;
	texturedesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureSRD;
	ZeroMemory(&textureSRD, sizeof(textureSRD));
	textureSRD.pSysMem = data;
	textureSRD.SysMemPitch = pw;

	HR(device->lpVtbl->CreateTexture2D(device, &texturedesc, &textureSRD, &texture));

	return texture;
}

static int newimage(lua_State* L) {
	void* mem = lua_tointeger(L, 1);
	lua_Integer w = luaL_checkinteger(L, 2);
	lua_Integer h = luaL_checkinteger(L, 3);
	lua_Integer pw = lua_tointeger(L, 4);

	ID3D11Texture2D* img = createImage(device, mem, w, h, pw);
	lua_pushinteger(L, img);

	return 1;
}

static int delimage(lua_State* L) {
	ID3D11Texture2D* img = lua_tointeger(L, 1);
	img->lpVtbl->Release(img);
	return 0;
}

static int putimage(lua_State* L) {
	ID3D11Texture2D* dst = lua_tointeger(L, 1);
	ID3D11Texture2D* src = lua_tointeger(L, 2);

	lua_Integer dx = lua_tointeger(L, 3);
	lua_Integer dy = lua_tointeger(L, 4);

	if (lua_gettop(L) == 4) {
		devicecontext->lpVtbl->CopySubresourceRegion(devicecontext, dst, 0, dx, dy, 0, src, 0, NULL);
	}
	else {
		lua_Integer sx = lua_tointeger(L, 5);
		lua_Integer sy = lua_tointeger(L, 6);
		lua_Integer sw = lua_tointeger(L, 7);
		lua_Integer sh = lua_tointeger(L, 8);
		D3D11_BOX box;
		box.left = sx; box.right = sx + sw;
		box.top = sy; box.bottom = sy + sh;
		box.front = 0; box.back = 1;
		devicecontext->lpVtbl->CopySubresourceRegion(devicecontext, dst, 0, dx, dy, 0, src, 0, &box);
	}

	return 0;
}

static int updimage(lua_State* L) {
	ID3D11Texture2D* dst = lua_tointeger(L, 1);

	lua_Integer dx = lua_tointeger(L, 2);
	lua_Integer dy = lua_tointeger(L, 3);
	lua_Integer dw = lua_tointeger(L, 4);
	lua_Integer dh = lua_tointeger(L, 5);

	void* mem = lua_tointeger(L, 6);

	lua_Integer pw = lua_tointeger(L, 7);

	D3D11_BOX box;
	box.left = dx; box.right = dx + dw;
	box.top = dy; box.bottom = dy + dh;
	box.front = 0; box.back = 1;
	devicecontext->lpVtbl->UpdateSubresource(devicecontext, dst, 0, &box, mem, pw, 0);

	return 0;
}

static const struct luaL_Reg imagelib[] = {
	{"create", newimage},
	{"delete", delimage},
	{"copy",   putimage},
	{"update", updimage},
	{NULL, NULL}
};

int luaopen_image(lua_State* L) {
	luaL_newlib(L, imagelib);
	return 1;
}
