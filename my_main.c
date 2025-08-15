#include <Windows.h>

#include "my_window.h"
#include "my_app.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, PWSTR pCmdLine, int nCmdShow) {
	//SYSTEMTIME time;
	//GetSystemTime(&time);
	//if (time.wYear > 2025 || time.wMonth > 7 || time.wDay > 27) {
	//	MessageBox(NULL, L"This HRAM beta version has expired, please get a new one, thanks!", L"HRAM beta version expired", 0);
	//	return 0;
	//}

	void* sysmem = VirtualAlloc(0x30000, 0x4000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!sysmem) {
		MessageBox(NULL, L"Could not allocate sufficient memory.", L"Fatal error", 0);
		return 1;
	}

	if (setupWindow(hInstance, nCmdShow)) {
		MessageBox(NULL, L"Could not setup window.", L"Fatal error", 0);
		return 1;
	}

	boot();
	runLoop();
	return 0;
}
