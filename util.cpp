#include "util.h"

#include <shlobj_core.h>
#include <atlstr.h>

std::string getUserDir() {
	PWSTR thepath;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &thepath);
	auto s = std::string(CW2A(thepath, CP_UTF8));
	CoTaskMemFree(thepath);
	return s;
}

std::string getExeName() {
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	return std::string(CW2A(szFileName, CP_UTF8));
}

void openConsole() {
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
}

void HR(HRESULT res) {
	if (res == S_OK) return;

	throw std::exception("can't create window");
}
