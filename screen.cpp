#include "Screen.h"

#include <stdint.h>
#include <vector>
#include <algorithm>

#include "image.h"
#include "util.h"

void Screen::setup(ID3D11Device* device) {
	std::vector<uint8_t> data(w * h * 4, 0x00);
	texture = createImage(device, data.data(), w, h);
	HR(device->CreateShaderResourceView(texture, nullptr, &texturesrv));
}
