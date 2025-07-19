#pragma once

#include <Windows.h>
#include <d3d11.h>
#include "screen.h"

int setupWindow(HINSTANCE hInstance, int nCmdShow);
void runLoop();

extern ID3D11Device* device;
extern ID3D11DeviceContext* devicecontext;

extern struct Screen* screen;
extern int screeni;
void useScreen(int n);
