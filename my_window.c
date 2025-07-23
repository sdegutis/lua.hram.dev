#include "my_window.h"

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxguid")

#include <windowsx.h>
#include <dwmapi.h>

#include "PixelShader.h"
#include "VertexShader.h"

#include "my_app.h"

WINDOWPLACEMENT lastwinpos = { sizeof(lastwinpos) };

HWND hwnd;
HWND hsub;

int padw;
int padh;

int subx;
int suby;
int subw;
int subh;

IDXGISwapChain* swapchain;
ID3D11Texture2D* framebuffer;
ID3D11RenderTargetView* framebufferRTV;
ID3D11RasterizerState* rasterizerstate;
ID3D11SamplerState* samplerstate;

ID3D11VertexShader* vertexshader;
ID3D11PixelShader* pixelshader;

void resetBuffers();
void moveSubWindow();


ID3D11Device* device;
ID3D11DeviceContext* devicecontext;

struct Screen screen = { 128, 72 };

int scale;
int winw;
int winh;

void draw();
void toggleFullscreen();
void setup_screen(ID3D11Device* device, struct Screen* scr);
ID3D11Texture2D* createImage(ID3D11Device* device, void* data, UINT w, UINT h, UINT pw);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#include "resource.h"

int setupWindow(HINSTANCE hInstance, int nCmdShow) {
	scale = 5;
	winw = screen.w * scale;
	winh = screen.h * scale;

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = WindowProc;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hInstance = hInstance;
	wc.lpszClassName = L"HRAM Window Class";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0x11, 0x11, 0x11));
	RegisterClass(&wc);

	RECT winbox;
	winbox.left = GetSystemMetrics(SM_CXSCREEN) / 2 - winw / 2;
	winbox.top = GetSystemMetrics(SM_CYSCREEN) / 2 - winh / 2;
	winbox.right = winbox.left + winw;
	winbox.bottom = winbox.top + winh;
	AdjustWindowRectEx(&winbox, WS_OVERLAPPEDWINDOW, 0, 0);

	padw = (winbox.right - winbox.left) - winw;
	padh = (winbox.bottom - winbox.top) - winh;

	hwnd = CreateWindowExW(
		0, L"HRAM Window Class", L"HRAM", WS_OVERLAPPEDWINDOW,
		winbox.left,
		winbox.top,
		winbox.right - winbox.left,
		winbox.bottom - winbox.top,
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) { return 1; }

	const BOOL isDarkMode = 1;
	HRESULT result = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkMode, sizeof(isDarkMode));

	WNDCLASS wc2;
	ZeroMemory(&wc2, sizeof(wc2));
	wc2.lpfnWndProc = WindowProc2;
	wc2.hInstance = hInstance;
	wc2.lpszClassName = L"HRAM SubWindow Class";
	RegisterClass(&wc2);

	moveSubWindow();
	hsub = CreateWindowExW(
		0, L"HRAM SubWindow Class", L"", WS_CHILD | WS_VISIBLE,
		subx, suby, subw, subh,
		hwnd, NULL, hInstance, NULL);
	if (hsub == NULL) { return 1; }

	DXGI_SWAP_CHAIN_DESC swapchaindesc;
	ZeroMemory(&swapchaindesc, sizeof(swapchaindesc));
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchaindesc.BufferCount = 2;
	swapchaindesc.OutputWindow = hsub;
	swapchaindesc.Windowed = TRUE;
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };
	HRESULT res;
	res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED, featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION, &swapchaindesc, &swapchain, &device, NULL, &devicecontext);
	if (res != S_OK) return 1;

	res = swapchain->lpVtbl->GetBuffer(swapchain, 0, &IID_ID3D11Texture2D, (void**)&framebuffer);
	if (res != S_OK || framebuffer == NULL) return 1;

	res = device->lpVtbl->CreateRenderTargetView(device, framebuffer, NULL, &framebufferRTV);
	if (res != S_OK) return 1;

	res = device->lpVtbl->CreateVertexShader(device, MyVertexShader, sizeof(MyVertexShader), 0, &vertexshader);
	if (res != S_OK) return 1;

	res = device->lpVtbl->CreatePixelShader(device, MyPixelShader, sizeof(MyPixelShader), 0, &pixelshader);
	if (res != S_OK) return 1;

	D3D11_RASTERIZER_DESC rasterizerdesc = { D3D11_FILL_SOLID, D3D11_CULL_BACK };
	res = device->lpVtbl->CreateRasterizerState(device, &rasterizerdesc, &rasterizerstate);
	if (res != S_OK) return 1;

	D3D11_SAMPLER_DESC samplerdesc = { D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP };
	res = device->lpVtbl->CreateSamplerState(device, &samplerdesc, &samplerstate);
	if (res != S_OK) return 1;

	D3D11_VIEWPORT viewport = { 0, 0, (float)subw, (float)subh, 0, 1 };
	devicecontext->lpVtbl->RSSetViewports(devicecontext, 1, &viewport);

	long len = screen.w * screen.h * 4;
	PUINT8 mem = HeapAlloc(GetProcessHeap(), 0, len);
	ZeroMemory(mem, len);
	screen.texture = createImage(device, mem, screen.w, screen.h, 0);
	HeapFree(GetProcessHeap(), 0, mem);
	res = device->lpVtbl->CreateShaderResourceView(device, screen.texture, NULL, &screen.texturesrv);
	if (res != S_OK) return 1;

	ShowWindow(hwnd, nCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	draw();

	return 0;
}

void runLoop() {
	DWORD last = 0, now, delta;
	MSG msg = { 0 };

	while (1) {
		Sleep(1);

		now = GetTickCount();
		delta = now - last;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) ExitProcess(0);
		}

		if (delta >= 30) {
			last = now;
			tick(delta, now);
		}
	}
}

inline void moveSubWindow() {
	subw = screen.w;
	subh = screen.h;
	scale = 1;

	while (
		subw + screen.w <= winw &&
		subh + screen.h <= winh)
	{
		subw += screen.w;
		subh += screen.h;
		scale++;
	}

	subx = winw / 2 - subw / 2;
	suby = winh / 2 - subh / 2;
}

void draw() {
	devicecontext->lpVtbl->IASetPrimitiveTopology(devicecontext, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	devicecontext->lpVtbl->VSSetShader(devicecontext, vertexshader, NULL, 0);

	devicecontext->lpVtbl->RSSetState(devicecontext, rasterizerstate);

	devicecontext->lpVtbl->PSSetShader(devicecontext, pixelshader, NULL, 0);
	devicecontext->lpVtbl->PSSetShaderResources(devicecontext, 0, 1, &screen.texturesrv);
	devicecontext->lpVtbl->PSSetSamplers(devicecontext, 0, 1, &samplerstate);

	devicecontext->lpVtbl->OMSetRenderTargets(devicecontext, 1, &framebufferRTV, NULL);

	devicecontext->lpVtbl->Draw(devicecontext, 4, 0);

	swapchain->lpVtbl->Present(swapchain, 1, 0);
}

void toggleFullscreen() {
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW) {
		MONITORINFO mi = { sizeof(mi) };
		if (GetWindowPlacement(hwnd, &lastwinpos) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi))
		{
			SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(hwnd, HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else {
		SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd, &lastwinpos);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

inline void resetBuffers() {
	D3D11_VIEWPORT viewport = { 0, 0, (float)subw, (float)subh, 0, 1 };
	devicecontext->lpVtbl->RSSetViewports(devicecontext, 1, &viewport);

	devicecontext->lpVtbl->Flush(devicecontext);

	framebufferRTV->lpVtbl->Release(framebufferRTV);
	framebuffer->lpVtbl->Release(framebuffer);

	swapchain->lpVtbl->ResizeBuffers(swapchain, 0, subw, subh, DXGI_FORMAT_UNKNOWN, 0);

	swapchain->lpVtbl->GetBuffer(swapchain, 0, &IID_ID3D11Texture2D, (void**)&framebuffer);
	device->lpVtbl->CreateRenderTargetView(device, framebuffer, NULL, &framebufferRTV);
}

ID3D11Texture2D* createImage(ID3D11Device* device, void* data, UINT w, UINT h, UINT pw) {
	if (pw == 0) pw = w;

	ID3D11Texture2D* texture = NULL;

	D3D11_TEXTURE2D_DESC texturedesc;
	ZeroMemory(&texturedesc, sizeof(texturedesc));
	texturedesc.Width = w;
	texturedesc.Height = h;
	texturedesc.MipLevels = 1;
	texturedesc.ArraySize = 1;
	texturedesc.Format = DXGI_FORMAT_R8_UNORM;
	texturedesc.SampleDesc.Count = 1;
	texturedesc.Usage = D3D11_USAGE_DEFAULT;
	texturedesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureSRD;
	ZeroMemory(&textureSRD, sizeof(textureSRD));
	textureSRD.pSysMem = data;
	textureSRD.SysMemPitch = pw;

	device->lpVtbl->CreateTexture2D(device, &texturedesc, &textureSRD, &texture);

	return texture;
}

void toggleFullscreen();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_SYSKEYUP:
		syskeyUp((int)wParam);
		return 0;

	case WM_SYSKEYDOWN:
		syskeyDown((int)wParam);
		if (wParam == VK_RETURN) { toggleFullscreen(); }
		return 0;

	case WM_SYSCHAR:    sysChar((const char)wParam); return 0;
	case WM_CHAR:       keyChar((const char)wParam); return 0;
	case WM_KEYUP:      keyUp((int)wParam);          return 0;
	case WM_KEYDOWN:    keyDown((int)wParam);        return 0;

	case WM_GETMINMAXINFO: {
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = screen.w + padw;
		lpMMI->ptMinTrackSize.y = screen.h + padh;
		return 0;
	}

	case WM_SIZE: {
		winw = LOWORD(lParam);
		winh = HIWORD(lParam);

		moveSubWindow();
		SetWindowPos(hsub, NULL, subx, suby, subw, subh, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		resetBuffers();

		draw();

		return 0;
	}

	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_SETCURSOR: {
		if (LOWORD(lParam) == HTCLIENT) {
			SetCursor(NULL);
			return TRUE;
		}
		break;
	}

	case WM_LBUTTONUP:   mouseUp(0);   return 0;
	case WM_LBUTTONDOWN: mouseDown(0); return 0;
	case WM_RBUTTONUP:   mouseUp(2);   return 0;
	case WM_RBUTTONDOWN: mouseDown(2); return 0;
	case WM_MBUTTONUP:   mouseUp(1);   return 0;
	case WM_MBUTTONDOWN: mouseDown(1); return 0;
	case WM_MOUSEWHEEL:  mouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)); return 0;

	case WM_MOUSEMOVE: {
		static int mousex;
		static int mousey;
		int x = GET_X_LPARAM(lParam) / scale;
		int y = GET_Y_LPARAM(lParam) / scale;
		if (x != mousex || y != mousey)
			mouseMoved(mousex = x, mousey = y);
		return 0;
	}

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
