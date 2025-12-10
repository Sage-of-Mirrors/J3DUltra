#pragma once

#include "J3D/Texture/J3DTexture.hpp"
#include "J3D/Util/J3DUtil.hpp"
#include "GX/GXEnum.hpp"

#include <cstdint>
#include <vector>
#include <memory>

struct J3DTexture;

class J3DTextureLoader {
public:
  J3DTextureLoader() {}
  ~J3DTextureLoader() {}

  std::shared_ptr<J3DTexture> Load(const std::string& textureName, bStream::CStream* stream);

  // Utility
  static void InitTexture(std::shared_ptr<J3DTexture> texture);
  static void SetTextureMipImage(uint32_t handle, uint32_t mipIdx, uint32_t mipWidth, uint32_t mipHeight, uint8_t* mipImg);

  static uint32_t GXWrapToGLWrap(EGXWrapMode gxWrap);
  static uint32_t GXFilterToGLFilter(EGXFilterMode gxFilter);
  static float GXAnisoToGLAniso(EGXMaxAnisotropy aniso);

protected:
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

  // Debug
  void OutputPNG(uint32_t index, std::shared_ptr<J3DTexture> texture);
};
