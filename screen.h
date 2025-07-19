#pragma once

#include <d3d11.h>

struct Screen {
	int w;
	int h;
	ID3D11ShaderResourceView* texturesrv;
	ID3D11Texture2D* texture;
};

void setup_screen(ID3D11Device* device, struct Screen* scr);
