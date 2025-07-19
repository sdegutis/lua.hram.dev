#include <Windows.h>

#include "window.h"
#include "app.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
	setupWindow(hInstance, nCmdShow);
	app::boot();
	runLoop();
	return 0;
}
