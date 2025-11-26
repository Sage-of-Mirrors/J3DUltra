#include "J3D/Texture/J3DTextureLoader.hpp"
#include "J3D/Data/J3DBlock.hpp"

#include "glad/glad.h"
#include "bstream.h"

#ifdef _DEBUG
#include <stb_image.h>
#include <stb_image_write.h>
#include <filesystem>
#endif

#include <cmath>

const float ONE_EIGHTH = 0.125f;
const float ONE_HUNDREDTH = 0.01f;

void J3DTextureLoader::InitTexture(std::shared_ptr<J3DTexture> texture) {
  texture->Clear();

  glCreateTextures(GL_TEXTURE_2D, 1, &texture->TexHandle);

  glTextureParameteri(texture->TexHandle, GL_TEXTURE_WRAP_S, GXWrapToGLWrap(texture->WrapS));
  glTextureParameteri(texture->TexHandle, GL_TEXTURE_WRAP_T, GXWrapToGLWrap(texture->WrapT));
  glTextureParameteri(texture->TexHandle, GL_TEXTURE_MIN_FILTER, GXFilterToGLFilter(texture->MinFilter));
  glTextureParameteri(texture->TexHandle, GL_TEXTURE_MAG_FILTER, GXFilterToGLFilter(texture->MagFilter));
  glTextureParameterf(texture->TexHandle, GL_TEXTURE_MIN_LOD, static_cast<float>(texture->MinLOD) * ONE_EIGHTH);
  glTextureParameterf(texture->TexHandle, GL_TEXTURE_MAX_LOD, static_cast<float>(texture->MaxLOD) * ONE_EIGHTH);
  glTextureParameterf(texture->TexHandle, GL_TEXTURE_LOD_BIAS, static_cast<float>(texture->LODBias) * ONE_HUNDREDTH);
  glTextureParameterf(texture->TexHandle, GL_TEXTURE_MAX_ANISOTROPY, GXAnisoToGLAniso(texture->MaxAnisotropy));

  glTextureStorage2D(texture->TexHandle, texture->MipmapCount, GL_RGBA8, texture->Width, texture->Height);
}

void J3DTextureLoader::SetTextureMipImage(uint32_t handle, uint32_t mipIdx, uint32_t mipWidth, uint32_t mipHeight, uint8_t* mipImg) {
  glTextureSubImage2D(handle, mipIdx, 0, 0, mipWidth, mipHeight, GL_RGBA, GL_UNSIGNED_BYTE, mipImg);
}

void J3DTextureLoader::DecodePaletteFormat(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData, uint32_t dataOffset, std::shared_ptr<J3DTexture> texture) {
  if (imageData == nullptr || !texture->PalettesEnabled)
    return;

  // Read palette data first.
  stream->seek(dataOffset + texture->PaletteOffset);

  uint32_t paletteDataSize = texture->PaletteCount * 2;
  uint8_t* paletteData = new uint8_t[texture->PaletteCount * 2];

  for (uint32_t i = 0; i < paletteDataSize; i++) {
    paletteData[i] = stream->readUInt8();
  }

  stream->seek(dataOffset + texture->TextureOffset);

  switch (texture->TextureFormat) {
  case EGXTextureFormat::C4:
    DecodeC4(stream, width, height, imageData, paletteData, texture->PaletteFormat);
    break;
  case EGXTextureFormat::C8:
    DecodeC8(stream, width, height, imageData, paletteData, texture->PaletteFormat);
    break;
  case EGXTextureFormat::C14X2:
  default:
    break;
  }

  delete[] paletteData;
}

std::shared_ptr<J3DTexture> J3DTextureLoader::Load(const std::string &textureName, bStream::CStream* stream) {
  uint32_t dataOffset = stream->tell();

  std::shared_ptr<J3DTexture> texture = std::make_shared<J3DTexture>();
  texture->Deserialize(stream);

  texture->Name = textureName;
  InitTexture(texture);

  // Load image data
  stream->seek(texture->TextureOffset);

  bool swapped = false;
  if (stream->getOrder() == bStream::Endianess::Little) {
    stream->setOrder(bStream::Endianess::Big);
    swapped = true;
  }

  for (int i = 0; i < texture->MipmapCount; i++) {
    uint16_t mipWidth = (uint16_t)(texture->Width / std::pow(2.0f, i));
    uint16_t mipHeight = (uint16_t)(texture->Height / std::pow(2.0f, i));

    uint8_t* imgData = new uint8_t[mipWidth * mipHeight * 4]{ };

    if (texture->TextureOffset != 0) {
      switch (texture->TextureFormat) {
      case EGXTextureFormat::I4:
        DecodeI4(stream, mipWidth, mipHeight, imgData);
        break;
      case EGXTextureFormat::I8:
        DecodeI8(stream, mipWidth, mipHeight, imgData);
        break;
      case EGXTextureFormat::IA4:
        DecodeIA4(stream, mipWidth, mipHeight, imgData);
        break;
      case EGXTextureFormat::IA8:
        DecodeIA8(stream, mipWidth, mipHeight, imgData);
        break;
      case EGXTextureFormat::RGB565:
        DecodeRGB565(stream, mipWidth, mipHeight, imgData);
        break;
      case EGXTextureFormat::RGB5A3:
        DecodeRGB5A3(stream, mipWidth, mipHeight, imgData);
        break;
      case EGXTextureFormat::RGBA32:
        DecodeRGBA32(stream, mipWidth, mipHeight, imgData);
        break;
      case EGXTextureFormat::C4:
      case EGXTextureFormat::C8:
      case EGXTextureFormat::C14X2:
        DecodePaletteFormat(stream, mipWidth, mipHeight, imgData, dataOffset, texture);
        break;
      case EGXTextureFormat::CMPR:
        DecodeCMPR(stream, mipWidth, mipHeight, imgData);
        break;
      }
    }

    texture->ImageData.push_back(imgData);
    SetTextureMipImage(texture->TexHandle, i, mipWidth, mipHeight, imgData);

#ifdef _DEBUG
    OutputPNG(i, texture);
#endif
  }

  if (swapped) {
    stream->setOrder(bStream::Endianess::Little);
  }

  texture->ImageData.shrink_to_fit();

  return texture;
}

void J3DTextureLoader::DecodeI4(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData) {
  if (imageData == nullptr)
    return;

  uint32_t numBlocksW = width / 8;
  uint32_t numBlocksH = height / 8;

  // Iterate the blocks in the image
  for (uint32_t blockY = 0; blockY < numBlocksH; blockY++) {
    for (uint32_t blockX = 0; blockX < numBlocksW; blockX++) {
      // Iterate the pixels in the current block
      for (uint32_t pixelY = 0; pixelY < 8; pixelY++) {
        for (uint32_t pixelX = 0; pixelX < 8; pixelX += 2) {
          // Bounds check to ensure the pixel is within the image.
          if ((blockX * 8 + pixelX >= width) || (blockY * 8 + pixelY >= height))
            continue;

          uint8_t data = stream->readUInt8();

          // Each byte represents two pixels.
          uint8_t pixel0 = (data & 0xF0) >> 4;
          uint8_t pixel1 = (data & 0x0F);

          uint32_t destIndex = (width * ((blockY * 8) + pixelY) + (blockX * 8) + pixelX) * 4;

          imageData[destIndex] = pixel0 * 0x11;
          imageData[destIndex + 1] = pixel0 * 0x11;
          imageData[destIndex + 2] = pixel0 * 0x11;
          imageData[destIndex + 3] = pixel0 * 0x11;

          imageData[destIndex + 4] = pixel1 * 0x11;
          imageData[destIndex + 5] = pixel1 * 0x11;
          imageData[destIndex + 6] = pixel1 * 0x11;
          imageData[destIndex + 7] = pixel1 * 0x11;
        }
      }
    }
  }
}

void J3DTextureLoader::DecodeI8(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData) {
  if (imageData == nullptr)
    return;

  uint32_t numBlocksW = width / 8;
  uint32_t numBlocksH = height / 4;

  // Iterate the blocks in the image
  for (uint32_t blockY = 0; blockY < numBlocksH; blockY++) {
    for (uint32_t blockX = 0; blockX < numBlocksW; blockX++) {
      // Iterate the pixels in the current block
      for (uint32_t pixelY = 0; pixelY < 4; pixelY++) {
        for (uint32_t pixelX = 0; pixelX < 8; pixelX++) {
          // Bounds check to ensure the pixel is within the image.
          if ((blockX * 8 + pixelX >= width) || (blockY * 4 + pixelY >= height))
            continue;

          uint8_t data = stream->readUInt8();

          uint32_t destIndex = (width * ((blockY * 4) + pixelY) + (blockX * 8) + pixelX) * 4;

          imageData[destIndex] = data;
          imageData[destIndex + 1] = data;
          imageData[destIndex + 2] = data;
          imageData[destIndex + 3] = data;
        }
      }
    }
  }
}

void J3DTextureLoader::DecodeIA4(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData) {
  if (imageData == nullptr)
    return;

  uint32_t numBlocksW = width / 8;
  uint32_t numBlocksH = height / 4;

  // Iterate the blocks in the image
  for (uint32_t blockY = 0; blockY < numBlocksH; blockY++) {
    for (uint32_t blockX = 0; blockX < numBlocksW; blockX++) {
      // Iterate the pixels in the current block
      for (uint32_t pixelY = 0; pixelY < 4; pixelY++) {
        for (uint32_t pixelX = 0; pixelX < 8; pixelX++) {
          // Bounds check to ensure the pixel is within the image.
          if ((blockX * 8 + pixelX >= width) || (blockY * 4 + pixelY >= height))
            continue;

          uint8_t data = stream->readUInt8();

          // Each byte contains alpha and luminance of the current pixel.
          uint8_t alpha = (data & 0xF0) >> 4;
          uint8_t luminance = (data & 0x0F);

          uint32_t destIndex = (width * ((blockY * 4) + pixelY) + (blockX * 8) + pixelX) * 4;

          imageData[destIndex] = luminance * 0x11;
          imageData[destIndex + 1] = luminance * 0x11;
          imageData[destIndex + 2] = luminance * 0x11;
          imageData[destIndex + 3] = alpha * 0x11;
        }
      }
    }
  }
}

void J3DTextureLoader::DecodeIA8(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData) {
  if (imageData == nullptr)
    return;

  uint32_t numBlocksW = width / 4;
  uint32_t numBlocksH = height / 4;

  // Iterate the blocks in the image
  for (uint32_t blockY = 0; blockY < numBlocksH; blockY++) {
    for (uint32_t blockX = 0; blockX < numBlocksW; blockX++) {
      // Iterate the pixels in the current block
      for (uint32_t pixelY = 0; pixelY < 4; pixelY++) {
        for (uint32_t pixelX = 0; pixelX < 4; pixelX++) {
          // Bounds check to ensure the pixel is within the image.
          if ((blockX * 4 + pixelX >= width) || (blockY * 4 + pixelY >= height))
            continue;

          // The alpha and luminance values of the current pixel are stored in two bytes.
          uint8_t alpha = stream->readUInt8();
          uint8_t luminance = stream->readUInt8();

          uint32_t destIndex = (width * ((blockY * 4) + pixelY) + (blockX * 4) + pixelX) * 4;

          imageData[destIndex] = luminance;
          imageData[destIndex + 1] = luminance;
          imageData[destIndex + 2] = luminance;
          imageData[destIndex + 3] = alpha;
        }
      }
    }
  }
}

void J3DTextureLoader::DecodeRGB565(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData) {
  if (imageData == nullptr)
    return;

  uint32_t numBlocksW = width / 4;
  uint32_t numBlocksH = height / 4;

  // Iterate the blocks in the image
  for (uint32_t blockY = 0; blockY < numBlocksH; blockY++) {
    for (uint32_t blockX = 0; blockX < numBlocksW; blockX++) {
      // Iterate the pixels in the current block
      for (uint32_t pixelY = 0; pixelY < 4; pixelY++) {
        for (uint32_t pixelX = 0; pixelX < 4; pixelX++) {
          // Bounds check to ensure the pixel is within the image.
          if ((blockX * 4 + pixelX >= width) || (blockY * 4 + pixelY >= height))
            continue;

          // RGB values for this pixel are stored in a 16-bit integer.
          uint16_t data = stream->readUInt16();
          uint32_t rgba8 = RGB565toRGBA8(data);

          uint32_t destIndex = (width * ((blockY * 4) + pixelY) + (blockX * 4) + pixelX) * 4;

          imageData[destIndex] = (rgba8 & 0xFF000000) >> 24;
          imageData[destIndex + 1] = (rgba8 & 0x00FF0000) >> 16;
          imageData[destIndex + 2] = (rgba8 & 0x0000FF00) >> 8;
          imageData[destIndex + 3] = rgba8 & 0x000000FF;
        }
      }
    }
  }
}

void J3DTextureLoader::DecodeRGB5A3(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData) {
  if (imageData == nullptr)
    return;

  uint32_t numBlocksW = width / 4;
  uint32_t numBlocksH = height / 4;

  // Iterate the blocks in the image
  for (uint32_t blockY = 0; blockY < numBlocksH; blockY++) {
    for (uint32_t blockX = 0; blockX < numBlocksW; blockX++) {
      // Iterate the pixels in the current block
      for (uint32_t pixelY = 0; pixelY < 4; pixelY++) {
        for (uint32_t pixelX = 0; pixelX < 4; pixelX++) {
          // Bounds check to ensure the pixel is within the image.
          if ((blockX * 4 + pixelX >= width) || (blockY * 4 + pixelY >= height))
            continue;

          // RGB values for this pixel are stored in a 16-bit integer.
          uint16_t data = stream->readUInt16();
          uint32_t rgba8 = RGB5A3toRGBA8(data);

          uint32_t destIndex = (width * ((blockY * 4) + pixelY) + (blockX * 4) + pixelX) * 4;

          imageData[destIndex] = (rgba8 & 0xFF000000) >> 24;
          imageData[destIndex + 1] = (rgba8 & 0x00FF0000) >> 16;
          imageData[destIndex + 2] = (rgba8 & 0x0000FF00) >> 8;
          imageData[destIndex + 3] = rgba8 & 0x000000FF;
        }
      }
    }
  }
}

void J3DTextureLoader::DecodeRGBA32(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData) {
  if (imageData == nullptr)
    return;

  uint32_t numBlocksW = width / 4;
  uint32_t numBlocksH = height / 4;

  // Iterate the blocks in the image
  for (uint32_t blockY = 0; blockY < numBlocksH; blockY++) {
    for (uint32_t blockX = 0; blockX < numBlocksW; blockX++) {
      // Iterate the pixels in the current block

      // Alpha/red values for current pixel
      for (uint32_t pixelY = 0; pixelY < 4; pixelY++) {
        for (uint32_t pixelX = 0; pixelX < 4; pixelX++) {
          // Bounds check to ensure the pixel is within the image.
          if ((blockX * 4 + pixelX >= width) || (blockY * 4 + pixelY >= height))
            continue;

          uint32_t destIndex = (width * ((blockY * 4) + pixelY) + (blockX * 4) + pixelX) * 4;

          imageData[destIndex + 3] = stream->readUInt8();
          imageData[destIndex] = stream->readUInt8();
        }
      }

      // Green/blue values for current pixel
      for (int pixelY = 0; pixelY < 4; pixelY++) {
        for (int pixelX = 0; pixelX < 4; pixelX++) {
          // Bounds check to ensure the pixel is within the image.
          if ((blockX * 4 + pixelX >= width) || (blockY * 4 + pixelY >= height))
            continue;

          uint32_t destIndex = (width * ((blockY * 4) + pixelY) + (blockX * 4) + pixelX) * 4;

          imageData[destIndex + 1] = stream->readUInt8();
          imageData[destIndex + 2] = stream->readUInt8();
        }
      }
    }
  }
}

void J3DTextureLoader::DecodeC4(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData, uint8_t* palette, EGXPaletteFormat paletteFormat) {
  if (imageData == nullptr || palette == nullptr)
    return;

  uint8_t* buffer = new uint8_t[width * height * 8];

  uint32_t numBlocksW = width / 8;
  uint32_t numBlocksH = height / 8;

  // Iterate the blocks in the image
  for (uint32_t blockY = 0; blockY < numBlocksH; blockY++) {
    for (uint32_t blockX = 0; blockX < numBlocksW; blockX++) {
      // Iterate the pixels in the current block
      for (uint32_t pixelY = 0; pixelY < 8; pixelY++) {
        for (uint32_t pixelX = 0; pixelX < 8; pixelX += 2) {
          // Bounds check to ensure the pixel is within the image.
          if ((blockX * 8 + pixelX >= width) || (blockY * 8 + pixelY >= height))
            continue;

          uint8_t data = stream->readUInt8();

          // Each byte contains the palette indices for 2 pixels.
          uint8_t pixel0 = (data & 0xF0) >> 4;
          uint8_t pixel1 = (data & 0x0F);

          uint32_t destIndex = (width * ((blockY * 8) + pixelY) + (blockX * 8) + pixelX);

          buffer[destIndex] = pixel0;
          buffer[destIndex + 1] = pixel1;
        }
      }
    }
  }

  uint32_t pixelSize = paletteFormat == EGXPaletteFormat::IA8 ? 2 : 4;
  uint32_t destOffset = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      UnpackPixelFromPalette(palette, buffer[y * width + x], imageData, destOffset, paletteFormat);
      destOffset += pixelSize;
    }
  }

  delete[] buffer;
}

void J3DTextureLoader::DecodeC8(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData, uint8_t* palette, EGXPaletteFormat paletteFormat) {
  if (imageData == nullptr || palette == nullptr)
    return;

  uint8_t* buffer = new uint8_t[width * height * 8];

  uint32_t numBlocksW = width / 8;
  uint32_t numBlocksH = height / 4;

  // Iterate the blocks in the image
  for (uint32_t blockY = 0; blockY < numBlocksH; blockY++) {
    for (uint32_t blockX = 0; blockX < numBlocksW; blockX++) {
      // Iterate the pixels in the current block
      for (uint32_t pixelY = 0; pixelY < 4; pixelY++) {
        for (uint32_t pixelX = 0; pixelX < 8; pixelX++) {
          // Bounds check to ensure the pixel is within the image.
          if ((blockX * 8 + pixelX >= width) || (blockY * 4 + pixelY >= height))
            continue;

          uint8_t data = stream->readUInt8();
          uint32_t destIndex = (width * ((blockY * 4) + pixelY) + (blockX * 8) + pixelX);

          buffer[destIndex] = data;
        }
      }
    }
  }

  uint32_t pixelSize = paletteFormat == EGXPaletteFormat::IA8 ? 2 : 4;
  uint32_t destOffset = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      UnpackPixelFromPalette(palette, buffer[y * width + x], imageData, destOffset, paletteFormat);
      destOffset += pixelSize;
    }
  }

  delete[] buffer;
}

void J3DTextureLoader::DecodeCMPR(bStream::CStream* stream, uint16_t width, uint16_t height, uint8_t* imageData) {
  if (imageData == nullptr)
    return;

  uint32_t numBlocksW = (width + 7) / 8;
  uint32_t numBlocksH = (height + 7) / 8;

  // Iterate the blocks in the image
  for (uint32_t blockY = 0; blockY < numBlocksH; blockY++) {
    for (uint32_t blockX = 0; blockX < numBlocksW; blockX++) {
      // Each block has a set of 2x2 sub-blocks.
      for (uint32_t subBlockY = 0; subBlockY < 2; subBlockY++) {
        for (uint32_t subBlockX = 0; subBlockX < 2; subBlockX++) {
          uint32_t subBlockWidth = std::max<uint32_t>(0, std::min<uint32_t>(4, width - (subBlockX * 4 + blockX * 8)));
          uint32_t subBlockHeight = std::max<uint32_t>(0, std::min<uint32_t>(4, height - (subBlockY * 4 + blockY * 8)));

          uint8_t* subBlockData = DecodeCMPRSubBlock(stream);

          for (uint32_t pixelY = 0; pixelY < subBlockHeight; pixelY++) {
            uint32_t destX = blockX * 8 + subBlockX * 4;
            uint32_t destY = blockY * 8 + (subBlockY * 4) + pixelY;

            if (destX >= width || destY >= height)
              continue;

            uint32_t destOffset = (destY * width + destX) * 4;
            memcpy(imageData + destOffset, subBlockData + (pixelY * 4 * 4), subBlockWidth * 4);
          }

          delete[] subBlockData;
        }
      }
    }
  }
}

uint8_t* J3DTextureLoader::DecodeCMPRSubBlock(bStream::CStream* stream) {
  uint8_t* data = new uint8_t[4 * 4 * 4]{};

  uint16_t color0 = stream->readUInt16();
  uint16_t color1 = stream->readUInt16();
  uint32_t bits = stream->readUInt32();

  uint32_t colorTable[4]{};
  colorTable[0] = RGB565toRGBA8(color0);
  colorTable[1] = RGB565toRGBA8(color1);

  uint8_t r0, g0, b0, a0, r1, g1, b1, a1;
  r0 = (colorTable[0] & 0xFF000000) >> 24;
  g0 = (colorTable[0] & 0x00FF0000) >> 16;
  b0 = (colorTable[0] & 0x0000FF00) >> 8;
  a0 = (colorTable[0] & 0x000000FF);

  r1 = (colorTable[1] & 0xFF000000) >> 24;
  g1 = (colorTable[1] & 0x00FF0000) >> 16;
  b1 = (colorTable[1] & 0x0000FF00) >> 8;
  a1 = (colorTable[1] & 0x000000FF);

  if (color0 > color1) {
    colorTable[2] |= ((2 * r0 + r1) / 3) << 24;
    colorTable[2] |= ((2 * g0 + g1) / 3) << 16;
    colorTable[2] |= ((2 * b0 + b1) / 3) << 8;
    colorTable[2] |= 0xFF;

    colorTable[3] |= ((r0 + 2 * r1) / 3) << 24;
    colorTable[3] |= ((g0 + 2 * g1) / 3) << 16;
    colorTable[3] |= ((b0 + 2 * b1) / 3) << 8;
    colorTable[3] |= 0xFF;
  }
  else {
    colorTable[2] |= ((r0 + r1) / 2) << 24;
    colorTable[2] |= ((g0 + g1) / 2) << 16;
    colorTable[2] |= ((b0 + b1) / 2) << 8;
    colorTable[2] |= 0xFF;

    colorTable[3] |= ((r0 + 2 * r1) / 3) << 24;
    colorTable[3] |= ((g0 + 2 * g1) / 3) << 16;
    colorTable[3] |= ((b0 + 2 * b1) / 3) << 8;
    colorTable[3] |= 0x00;
  }

  for (int pixelY = 0; pixelY < 4; pixelY++) {
    for (int pixelX = 0; pixelX < 4; pixelX++) {
      uint32_t i = pixelY * 4 + pixelX;
      uint32_t bitOffset = (15 - i) * 2;
      uint32_t di = i * 4;
      uint32_t si = (bits >> bitOffset) & 3;

      data[di + 0] = (colorTable[si] & 0xFF000000) >> 24;
      data[di + 1] = (colorTable[si] & 0x00FF0000) >> 16;
      data[di + 2] = (colorTable[si] & 0x0000FF00) >> 8;
      data[di + 3] = (colorTable[si] & 0x000000FF);
    }
  }

  return data;
}

void J3DTextureLoader::OutputPNG(uint32_t index, std::shared_ptr<J3DTexture> texture) {
#ifdef _DEBUG
  uint16_t mip_width = (texture->Width >> index);
  uint16_t mip_height = (texture->Height >> index);
  if (mip_width == 0 || mip_height == 0) {
    return;
  }

  if (!std::filesystem::is_directory("./texturedump")) {
    std::filesystem::create_directory("./texturedump");
  }

  std::string fileName = "./texturedump/" + texture->Name + "_mip" + std::to_string(index) + ".png";

  stbi_write_png(fileName.c_str(), mip_width, mip_height, 4, texture->ImageData[index], mip_width * 4);
#endif
}

void J3DTextureLoader::UnpackPixelFromPalette(uint8_t* palette, uint32_t index, uint8_t* dest, uint32_t offset, EGXPaletteFormat format) {
  switch (format) {
  case EGXPaletteFormat::IA8:
    dest[offset] = palette[2 * index + 1];
    dest[offset + 1] = palette[2 * index];
    break;
  case EGXPaletteFormat::RGB565:
  {
    uint16_t paletteColor = (palette[2 * index]) << 8 | (palette[2 * index + 1]);
    uint32_t rgba8 = RGB565toRGBA8(paletteColor);

    dest[offset] = (rgba8 & 0xFF000000) >> 24;
    dest[offset + 1] = (rgba8 & 0x00FF0000) >> 16;
    dest[offset + 2] = (rgba8 & 0x0000FF00) >> 8;
    dest[offset + 3] = rgba8 & 0x000000FF;
    break;
  }
  case EGXPaletteFormat::RGB5A3:
  {
    uint16_t paletteColor = (palette[2 * index]) << 8 | (palette[2 * index + 1]);
    uint32_t rgba8 = RGB5A3toRGBA8(paletteColor);

    dest[offset] = (rgba8 & 0xFF000000) >> 24;
    dest[offset + 1] = (rgba8 & 0x00FF0000) >> 16;
    dest[offset + 2] = (rgba8 & 0x0000FF00) >> 8;
    dest[offset + 3] = rgba8 & 0x000000FF;
    break;
  }
  }
}

uint32_t J3DTextureLoader::RGB565toRGBA8(uint16_t data) {
  uint8_t r = (data & 0xF100) >> 11;
  uint8_t g = (data & 0x07E0) >> 5;
  uint8_t b = (data & 0x001F);

  uint32_t output = 0x000000FF;
  output |= (r << 3) << 24;
  output |= (g << 2) << 16;
  output |= (b << 3) << 8;

  return output;
}

uint32_t J3DTextureLoader::RGB5A3toRGBA8(uint16_t data) {
  uint8_t r, g, b, a;

  // No alpha bits to extract.
  if (data & 0x8000) {
    a = 0xFF;

    r = (data & 0x7C00) >> 10;
    g = (data & 0x03E0) >> 5;
    b = (data & 0x001F);

    r = (r << (8 - 5)) | (r >> (10 - 8));
    g = (g << (8 - 5)) | (g >> (10 - 8));
    b = (b << (8 - 5)) | (b >> (10 - 8));
  }
  // Alpha bits present.
  else {
    a = (data & 0x7000) >> 12;
    r = (data & 0x0F00) >> 8;
    g = (data & 0x00F0) >> 4;
    b = (data & 0x000F);

    a = (a << (8 - 3)) | (a << (8 - 6)) | (a >> (9 - 8));
    r = (r << (8 - 4)) | r;
    g = (g << (8 - 4)) | g;
    b = (b << (8 - 4)) | b;
  }

  uint32_t output = a;
  output |= r << 24;
  output |= g << 16;
  output |= b << 8;

  return output;
}

uint32_t J3DTextureLoader::GXWrapToGLWrap(EGXWrapMode gxWrap) {
  switch (gxWrap) {
  case EGXWrapMode::ClampToEdge:
    return GL_CLAMP_TO_EDGE;
  case EGXWrapMode::MirroredRepeat:
    return GL_MIRRORED_REPEAT;
  case EGXWrapMode::Repeat:
    return GL_REPEAT;
  default:
    return GL_CLAMP_TO_EDGE;
  }
}

uint32_t J3DTextureLoader::GXFilterToGLFilter(EGXFilterMode gxFilter) {
  switch (gxFilter) {
  case EGXFilterMode::Nearest:
    return GL_NEAREST;
  case EGXFilterMode::Linear:
    return GL_LINEAR;
  case EGXFilterMode::NearestMipmapNearest:
    return GL_NEAREST_MIPMAP_NEAREST;
  case EGXFilterMode::NearestMipmapLinear:
    return GL_NEAREST_MIPMAP_LINEAR;
  case EGXFilterMode::LinearMipmapNearest:
    return GL_LINEAR_MIPMAP_NEAREST;
  case EGXFilterMode::LinearMipmapLinear:
    return GL_LINEAR_MIPMAP_LINEAR;
  default:
    return GL_NEAREST;
  }
}

float J3DTextureLoader::GXAnisoToGLAniso(EGXMaxAnisotropy aniso) {
  switch (aniso) {
  case EGXMaxAnisotropy::One:
    return 1.0f;
  case EGXMaxAnisotropy::Two:
    return 2.0f;
  case EGXMaxAnisotropy::Four:
    return 4.0f;
  }

  return 1.0f;
}
