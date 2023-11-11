#pragma once

#include "GX/GXEnum.hpp"

#include <string>
#include <cstdint>
#include <memory>
#include <vector>

namespace bStream { class CStream; }

struct J3DTexture {
	// General data
	EGXTextureFormat TextureFormat;
	bool AlphaEnabled;
	uint16_t Width;
	uint16_t Height;
	EGXWrapMode WrapS;
	EGXWrapMode WrapT;

	// Palette data
	bool PalettesEnabled;
	EGXPaletteFormat PaletteFormat;
	uint16_t PaletteCount;
	uint32_t PaletteOffset;

	// Etc. data
	bool MipmapsEnabled;
	bool DoEdgeLOD;
	bool BiasClamp;
	EGXMaxAnisotropy MaxAnisotropy;
	EGXFilterMode MinFilter;
	EGXFilterMode MagFilter;

	int MinLOD;
	int MaxLOD;
	int MipmapCount;
	int Unknown;
	int LODBias;
	int TextureOffset;

	std::string Name;
	std::vector<uint8_t*> ImageData;
	uint32_t TexHandle;

	J3DTexture();
	~J3DTexture();

	void Deserialize(bStream::CStream* stream);
};
