#include "image.h"

#include "util.h"
#include "window.h"

ID3D11Texture2D* createImage(ID3D11Device* device, void* data, int w, int h, int pw) {
	if (pw == 0) pw = w * 4;

	ID3D11Texture2D* texture = nullptr;

	D3D11_TEXTURE2D_DESC texturedesc = {};
	texturedesc.Width = w;
	texturedesc.Height = h;
	texturedesc.MipLevels = 1;
	texturedesc.ArraySize = 1;
	texturedesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texturedesc.SampleDesc.Count = 1;
	texturedesc.Usage = D3D11_USAGE_DEFAULT;
	texturedesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureSRD = {};
	textureSRD.pSysMem = data;
	textureSRD.SysMemPitch = pw;

	HR(device->CreateTexture2D(&texturedesc, &textureSRD, &texture));

	return texture;
}

static int newimage(lua_State* L) {
	auto mem = reinterpret_cast<void*>(lua_tointeger(L, 1));
	auto w = luaL_checkinteger(L, 2);
	auto h = luaL_checkinteger(L, 3);
	auto pw = lua_tointeger(L, 4);

	auto img = createImage(device, mem, w, h, pw);
	lua_pushinteger(L, reinterpret_cast<uint64_t>(img));

	return 1;
}

static int delimage(lua_State* L) {
	auto img = reinterpret_cast<ID3D11Texture2D*>(lua_tointeger(L, 1));
	img->Release();
	return 0;
}

static int putimage(lua_State* L) {
	auto dst = reinterpret_cast<ID3D11Texture2D*>(lua_tointeger(L, 1));
	auto src = reinterpret_cast<ID3D11Texture2D*>(lua_tointeger(L, 2));
	if (dst == nullptr) dst = screen->texture;

	auto dx = lua_tointeger(L, 3);
	auto dy = lua_tointeger(L, 4);

	if (lua_gettop(L) == 4) {
		devicecontext->CopySubresourceRegion(dst, 0, dx, dy, 0, src, 0, NULL);
	}
	else {
		auto sx = lua_tointeger(L, 5);
		auto sy = lua_tointeger(L, 6);
		auto sw = lua_tointeger(L, 7);
		auto sh = lua_tointeger(L, 8);
		D3D11_BOX box;
		box.left = sx; box.right = sx + sw;
		box.top = sy; box.bottom = sy + sh;
		box.front = 0; box.back = 1;
		devicecontext->CopySubresourceRegion(dst, 0, dx, dy, 0, src, 0, &box);
	}

	return 0;
}

static int updimage(lua_State* L) {
	auto dst = reinterpret_cast<ID3D11Texture2D*>(lua_tointeger(L, 1));
	if (dst == nullptr) dst = screen->texture;

	auto dx = lua_tointeger(L, 2);
	auto dy = lua_tointeger(L, 3);
	auto dw = lua_tointeger(L, 4);
	auto dh = lua_tointeger(L, 5);

	auto mem = reinterpret_cast<void*>(lua_tointeger(L, 6));

	auto pw = lua_tointeger(L, 7);

	D3D11_BOX box;
	box.left = dx; box.right = dx + dw;
	box.top = dy; box.bottom = dy + dh;
	box.front = 0; box.back = 1;
	devicecontext->UpdateSubresource(dst, 0, &box, mem, pw, 0);

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
