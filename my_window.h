#pragma once

#include <Windows.h>
#include <d3d11.h>

struct Screen {
	int w;
	int h;
	ID3D11ShaderResourceView* texturesrv;
	ID3D11Texture2D* texture;
};

int setupWindow(HINSTANCE hInstance, int nCmdShow);
void runLoop();

extern ID3D11Device* device;
extern ID3D11DeviceContext* devicecontext;

extern struct Screen* screen;
extern int screeni;
void useScreen(int n);
