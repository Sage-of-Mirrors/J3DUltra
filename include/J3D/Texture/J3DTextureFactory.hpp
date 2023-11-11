#pragma once

#include "J3DTexture.hpp"
#include "J3D/Util/J3DNameTable.hpp"
#include "GX/GXEnum.hpp"

#include <vector>
#include <memory>
#include <cstdint>

namespace bStream { class CStream; }
struct J3DTextureBlock;

constexpr uint32_t TEXTURE_ENTRY_SIZE = 32;

class J3DTextureFactory {
	J3DTextureBlock* mBlock;
	J3DNameTable mNameTable;

	// Greyscale formats
	void DecodeI4(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData);
	void DecodeI8(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData);
	void DecodeIA4(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData);
	void DecodeIA8(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData);

	// Color formats
	void DecodeRGB565(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData);
	void DecodeRGB5A3(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData);
	void DecodeRGBA32(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData);

	uint32_t RGB565toRGBA8(uint16_t data);
	uint32_t RGB5A3toRGBA8(uint16_t data);

	// Palette formats
	void DecodePaletteFormat(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData, uint32_t dataOffset, std::shared_ptr<J3DTexture> texture);
	void DecodeC4(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData, uint8_t* palette, EGXPaletteFormat paletteFormat);
	void DecodeC8(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData, uint8_t* palette, EGXPaletteFormat paletteFormat);

	void UnpackPixelFromPalette(uint8_t* palette, uint32_t index, uint8_t* dest, uint32_t offset, EGXPaletteFormat format);

	// CMPR
	void DecodeCMPR(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData);
	uint8_t* DecodeCMPRSubBlock(bStream::CStream* stream);

	// Utility
	uint32_t GXWrapToGLWrap(EGXWrapMode gxWrap);
	uint32_t GXFilterToGLFilter(EGXFilterMode gxFilter);
	float GXAnisoToGLAniso(EGXMaxAnisotropy aniso);

	// Debug
	void OutputPNG(uint32_t index, std::shared_ptr<J3DTexture> texture);

public:
	J3DTextureFactory(J3DTextureBlock* srcBlock, bStream::CStream* stream);
	~J3DTextureFactory() {}

	std::shared_ptr<J3DTexture> Create(bStream::CStream* stream, uint32_t index);
};
