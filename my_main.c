#include <Windows.h>

#include "my_window.h"
#include "my_app.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, PWSTR pCmdLine, int nCmdShow) {
	if (setupWindow(hInstance, nCmdShow)) return 1;
	boot();
	runLoop();
	return 0;
}
