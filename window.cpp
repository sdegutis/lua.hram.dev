#include "window.h"

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")

#include <windowsx.h>
#include <dwmapi.h>

#include <exception>
#include <vector>

#include "PixelShader.h"
#include "VertexShader.h"

#include "screen.h"
#include "app.h"
#include "util.h"

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



std::vector<Screen> screens{
	{ 320, 180 },
	{ 320, 200 },
};
int screeni = 0;
Screen* screen = &screens[screeni];

void useScreen(int n) {
	screeni = n;
	screen = &screens[screeni];

	moveSubWindow();
	SetWindowPos(hsub, NULL, subx, suby, subw, subh, SWP_FRAMECHANGED);
	resetBuffers();
}



int scale = 3;
int winw = screen->w * scale;
int winh = screen->h * scale;

void draw();

void toggleFullscreen();



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void setupWindow(HINSTANCE hInstance, int nCmdShow) {

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
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
	AdjustWindowRectEx(&winbox, WS_OVERLAPPEDWINDOW, false, 0);

	padw = (winbox.right - winbox.left) - winw;
	padh = (winbox.bottom - winbox.top) - winh;

	hwnd = CreateWindowExW(
		0, L"HRAM Window Class", L"HRAM", WS_OVERLAPPEDWINDOW,
		winbox.left,
		winbox.top,
		winbox.right - winbox.left,
		winbox.bottom - winbox.top,
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) { throw std::exception("can't create window"); }

	const BOOL isDarkMode = true;
	HRESULT result = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkMode, sizeof(isDarkMode));

	WNDCLASS wc2 = {};
	wc2.lpfnWndProc = WindowProc2;
	wc2.hInstance = hInstance;
	wc2.lpszClassName = L"HRAM SubWindow Class";
	RegisterClass(&wc2);

	moveSubWindow();
	hsub = CreateWindowExW(
		0, L"HRAM SubWindow Class", L"", WS_CHILD | WS_VISIBLE,
		subx, suby, subw, subh,
		hwnd, NULL, hInstance, NULL);
	if (hsub == NULL) { throw std::exception("can't create canvas view"); }

	DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchaindesc.BufferCount = 2;
	swapchaindesc.OutputWindow = hsub;
	swapchaindesc.Windowed = TRUE;
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };
	HR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED, featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION, &swapchaindesc, &swapchain, &device, nullptr, &devicecontext));

	HR(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer));
	if (framebuffer == NULL) { throw std::exception("can't create canvas view"); }
	HR(device->CreateRenderTargetView(framebuffer, nullptr, &framebufferRTV));

	HR(device->CreateVertexShader(MyVertexShader, sizeof(MyVertexShader), 0, &vertexshader));
	HR(device->CreatePixelShader(MyPixelShader, sizeof(MyPixelShader), 0, &pixelshader));

	D3D11_RASTERIZER_DESC rasterizerdesc = { D3D11_FILL_SOLID, D3D11_CULL_BACK };
	HR(device->CreateRasterizerState(&rasterizerdesc, &rasterizerstate));

	D3D11_SAMPLER_DESC samplerdesc = { D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP };
	HR(device->CreateSamplerState(&samplerdesc, &samplerstate));

	D3D11_VIEWPORT viewport = { 0, 0, (float)subw, (float)subh, 0, 1 };
	devicecontext->RSSetViewports(1, &viewport);

	for (auto& s : screens) {
		s.setup(device);
	}

	ShowWindow(hwnd, nCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
}

void runLoop() {

	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			draw();
		}
	}


}

inline void moveSubWindow() {
	subw = screen->w;
	subh = screen->h;
	scale = 1;

	while (
		subw + screen->w <= winw &&
		subh + screen->h <= winh)
	{
		subw += screen->w;
		subh += screen->h;
		scale++;
	}

	subx = winw / 2 - subw / 2;
	suby = winh / 2 - subh / 2;
}

inline void draw() {
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	devicecontext->VSSetShader(vertexshader, nullptr, 0);

	devicecontext->RSSetState(rasterizerstate);

	devicecontext->PSSetShader(pixelshader, nullptr, 0);
	devicecontext->PSSetShaderResources(0, 1, &screen->texturesrv);
	devicecontext->PSSetSamplers(0, 1, &samplerstate);

	devicecontext->OMSetRenderTargets(1, &framebufferRTV, nullptr);

	devicecontext->Draw(4, 0);

	swapchain->Present(1, 0);
}

inline void toggleFullscreen() {
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
	devicecontext->RSSetViewports(1, &viewport);

	devicecontext->Flush();

	framebufferRTV->Release();
	framebuffer->Release();

	HR(swapchain->ResizeBuffers(0, subw, subh, DXGI_FORMAT_UNKNOWN, 0));


	HR(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer));
	HR(device->CreateRenderTargetView(framebuffer, nullptr, &framebufferRTV));
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_CHAR:       app::keyChar((const char)wParam); return 0;
	case WM_SYSKEYDOWN: app::keyDown((int)wParam);        return 0;
	case WM_KEYUP:      app::keyUp((int)wParam);          return 0;
	case WM_KEYDOWN:    app::keyDown((int)wParam);        return 0;
	case WM_SYSKEYUP:   app::keyUp((int)wParam);          return 0;

	case WM_GETMINMAXINFO: {
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = screen->w + padw;
		lpMMI->ptMinTrackSize.y = screen->h + padh;
		return 0;
	}

	case WM_SIZE: {
		winw = LOWORD(lParam);
		winh = HIWORD(lParam);

		moveSubWindow();
		SetWindowPos(hsub, NULL, subx, suby, subw, subh, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		resetBuffers();

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

	case WM_LBUTTONUP:   app::mouseUp(0);   return 0;
	case WM_LBUTTONDOWN: app::mouseDown(0); return 0;
	case WM_RBUTTONUP:   app::mouseUp(2);   return 0;
	case WM_RBUTTONDOWN: app::mouseDown(2); return 0;
	case WM_MBUTTONUP:   app::mouseUp(1);   return 0;
	case WM_MBUTTONDOWN: app::mouseDown(1); return 0;
	case WM_MOUSEWHEEL:  app::mouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)); return 0;

	case WM_MOUSEMOVE: {
		static int mousex;
		static int mousey;
		auto x = GET_X_LPARAM(lParam) / scale;
		auto y = GET_Y_LPARAM(lParam) / scale;
		if (x != mousex || y != mousey)
			app::mouseMoved(mousex = x, mousey = y);
		return 0;
	}

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
