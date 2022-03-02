#pragma once

#include "J3DNameTable.hpp"
#include "GX/GXEnum.hpp"

#include <vector>
#include <cstdint>

namespace bStream { class CStream; }
class J3DTextureBlock;

constexpr uint32_t TEXTURE_ENTRY_SIZE = 32;

struct J3DTextureEntry {
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
	uint8_t MaxAnisotropy;
	EGXFilterMode MinFilter;
	EGXFilterMode MagFilter;

	uint8_t MinLOD;
	uint8_t MaxLOD;
	uint8_t MipmapCount;
	uint8_t Unknown;
	uint16_t LODBias;
	uint32_t TextureOffset;

	void Deserialize(bStream::CStream* stream);
};

class J3DTextureFactory {
	J3DTextureBlock* mBlock;
	J3DNameTable mNameTable;

	// Greyscale formats
	void DecodeI4(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry);
	void DecodeI8(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry);
	void DecodeIA4(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry);
	void DecodeIA8(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry);

	// Color formats
	void DecodeRGB565(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry);
	void DecodeRGB5A3(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry);
	void DecodeRGBA32(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry);

	uint32_t RGB565toRGBA8(uint16_t data);
	uint32_t RGB5A3toRGBA8(uint16_t data);

	// Palette formats
	void DecodePaletteFormat(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry, uint32_t dataOffset);
	void DecodeC4(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry, uint8_t* palette);
	void DecodeC8(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry, uint8_t* palette);

	void UnpackPixelFromPalette(uint8_t* palette, uint32_t index, uint8_t* dest, uint32_t offset, EGXPaletteFormat format);

	// CMPR
	void DecodeCMPR(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry);
	uint8_t* DecodeCMPRSubBlock(bStream::CStream* stream);

	// Utility
	uint32_t GXWrapToGLWrap(EGXWrapMode gxWrap);
	uint32_t GXFilterToGLFilter(EGXFilterMode gxFilter);

	// Debug
	void OutputPNG(uint32_t index, uint8_t* imageData, J3DTextureEntry& entry);

public:
	J3DTextureFactory(J3DTextureBlock* srcBlock, bStream::CStream* stream);
	~J3DTextureFactory() {}

	uint32_t Create(bStream::CStream* stream, uint32_t index);
};
