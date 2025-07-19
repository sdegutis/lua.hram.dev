#include "Screen.h"

#include <stdint.h>

#include "image.h"
#include "util.h"

void setup_screen(ID3D11Device* device, struct Screen* scr) {
	long len = scr->w * scr->h * 4;
	uint8_t* mem = HeapAlloc(GetProcessHeap(), 0, len);
	ZeroMemory(mem, len);
	scr->texture = createImage(device, mem, scr->w, scr->h, 0);
	HR(device->lpVtbl->CreateShaderResourceView(device, scr->texture, NULL, &scr->texturesrv));
}
