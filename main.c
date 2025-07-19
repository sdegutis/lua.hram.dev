#include <Windows.h>

#include "window.h"
#include "app.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, PWSTR pCmdLine, int nCmdShow) {
	if (setupWindow(hInstance, nCmdShow)) return 1;
	boot();
	runLoop();
	return 0;
}
