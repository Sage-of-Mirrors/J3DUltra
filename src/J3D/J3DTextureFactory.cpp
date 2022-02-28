#include "J3D/J3DTextureFactory.hpp"
#include "J3D/J3DBlock.hpp"
#include "png.hpp"

#include "bstream.h"

void J3DTextureEntry::Deserialize(bStream::CStream* stream) {
	TextureFormat = (EGXTextureFormat)stream->readUInt8();
	AlphaEnabled = stream->readUInt8();
	Width = stream->readUInt16();
	Height = stream->readUInt16();
	WrapS = (EGXWrapMode)stream->readUInt8();
	WrapT = (EGXWrapMode)stream->readUInt8();

	PalettesEnabled = stream->readUInt8();
	PaletteFormat = (EGXPaletteFormat)stream->readUInt8();
	PaletteCount = stream->readUInt16();
	PaletteOffset = stream->readUInt32();

	MipmapsEnabled = stream->readUInt8();
	DoEdgeLOD = stream->readUInt8();
	BiasClamp = stream->readUInt8();
	MaxAnisotropy = stream->readUInt8();
	MinFilter = (EGXFilterMode)stream->readUInt8();
	MagFilter = (EGXFilterMode)stream->readUInt8();

	MinLOD = stream->readUInt8();
	MaxLOD = stream->readUInt8();
	MipmapCount = stream->readUInt8();
	Unknown = stream->readUInt8();
	LODBias = stream->readUInt16();
	TextureOffset = stream->readUInt32();
}

void J3DTextureFactory::OutputPNG(uint32_t index, uint8_t* imageData, J3DTextureEntry& entry) {
	std::string texName = mNameTable.GetName(index);

	png::image<png::rgba_pixel> image(entry.Width, entry.Height);

	for (int y = 0; y < entry.Height; y++) {
		for (int x = 0; x < entry.Width; x++) {
			uint32_t px = (y * entry.Width + x) * 4;
			image[y][x] = png::rgba_pixel(imageData[px], imageData[px + 1], imageData[px + 2], imageData[px + 3]);
		}
	}

	image.write("./" + texName + ".png");
}

J3DTextureFactory::J3DTextureFactory(J3DTextureBlock* srcBlock, bStream::CStream* stream) {
	mBlock = srcBlock;

	stream->seek(mBlock->NameTableOffset);
	mNameTable.Deserialize(stream);
}

uint32_t J3DTextureFactory::Create(bStream::CStream* stream, uint32_t index) {
	uint32_t dataOffset = mBlock->TexTableOffset + (index * TEXTURE_ENTRY_SIZE);
	stream->seek(dataOffset);

	J3DTextureEntry entry;
	entry.Deserialize(stream);

	uint8_t* imageData = new uint8_t[entry.Width * entry.Height * 4];

	switch (entry.TextureFormat) {
		case EGXTextureFormat::I4:
			stream->seek(dataOffset + entry.TextureOffset);
			DecodeI4(stream, imageData, entry);
			break;
		case EGXTextureFormat::I8:
			stream->seek(dataOffset + entry.TextureOffset);
			DecodeI8(stream, imageData, entry);
			break;
		case EGXTextureFormat::IA4:
			stream->seek(dataOffset + entry.TextureOffset);
			DecodeIA4(stream, imageData, entry);
			break;
		case EGXTextureFormat::IA8:
			stream->seek(dataOffset + entry.TextureOffset);
			DecodeIA8(stream, imageData, entry);
			break;
		case EGXTextureFormat::RGB565:
			stream->seek(dataOffset + entry.TextureOffset);
			DecodeRGB565(stream, imageData, entry);
			break;
		case EGXTextureFormat::RGB5A3:
			stream->seek(dataOffset + entry.TextureOffset);
			DecodeRGB5A3(stream, imageData, entry);
			break;
		case EGXTextureFormat::RGBA32:
			stream->seek(dataOffset + entry.TextureOffset);
			DecodeRGBA32(stream, imageData, entry);
			break;
		case EGXTextureFormat::C4:
		case EGXTextureFormat::C8:
		case EGXTextureFormat::C14X2:
			DecodePaletteFormat(stream, imageData, entry, dataOffset);
			break;
		case EGXTextureFormat::CMPR:
			DecodeCMPR(stream, imageData, entry);
			break;
	}

	OutputPNG(index, imageData, entry);

	delete[] imageData;

	return 0;
}

void J3DTextureFactory::DecodePaletteFormat(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry, uint32_t dataOffset) {
	if (imageData == nullptr || !entry.PalettesEnabled)
		return;

	// Read palette data first.
	stream->seek(dataOffset + entry.PaletteOffset);

	uint32_t paletteDataSize = entry.PaletteCount * 2;
	uint8_t* paletteData = new uint8_t[entry.PaletteCount * 2];

	for (int i = 0; i < paletteDataSize; i++) {
		paletteData[i] = stream->readUInt8();
	}

	stream->seek(dataOffset + entry.TextureOffset);

	switch (entry.TextureFormat) {
		case EGXTextureFormat::C4:
			DecodeC4(stream, imageData, entry, paletteData);
			break;
		case EGXTextureFormat::C8:
			DecodeC8(stream, imageData, entry, paletteData);
			break;
		case EGXTextureFormat::C14X2:
		default:
			break;
	}

	delete[] paletteData;
}

void J3DTextureFactory::DecodeI4(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry) {
	if (imageData == nullptr)
		return;

	uint32_t numBlocksW = entry.Width / 8;
	uint32_t numBlocksH = entry.Height / 8;
	
	// Iterate the blocks in the image
	for (int blockY = 0; blockY < numBlocksH; blockY++) {
		for (int blockX = 0; blockX < numBlocksW; blockX++) {
			// Iterate the pixels in the current block
			for (int pixelY = 0; pixelY < 8; pixelY++) {
				for (int pixelX = 0; pixelX < 8; pixelX += 2) {
					// Bounds check to ensure the pixel is within the image.
					if ((blockX * 8 + pixelX >= entry.Width) || (blockY * 8 + pixelY >= entry.Height))
						continue;

					uint8_t data = stream->readUInt8();

					// Each byte represents two pixels.
					uint8_t pixel0 = (data & 0xF0) >> 4;
					uint8_t pixel1 = (data & 0x0F);

					uint32_t destIndex = (entry.Width * ((blockY * 8) + pixelY) + (blockX * 8) + pixelX) * 4;

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

void J3DTextureFactory::DecodeI8(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry) {
	if (imageData == nullptr)
		return;

	uint32_t numBlocksW = entry.Width / 8;
	uint32_t numBlocksH = entry.Height / 4;

	// Iterate the blocks in the image
	for (int blockY = 0; blockY < numBlocksH; blockY++) {
		for (int blockX = 0; blockX < numBlocksW; blockX++) {
			// Iterate the pixels in the current block
			for (int pixelY = 0; pixelY < 4; pixelY++) {
				for (int pixelX = 0; pixelX < 8; pixelX++) {
					// Bounds check to ensure the pixel is within the image.
					if ((blockX * 8 + pixelX >= entry.Width) || (blockY * 4 + pixelY >= entry.Height))
						continue;

					uint8_t data = stream->readUInt8();

					uint32_t destIndex = (entry.Width * ((blockY * 4) + pixelY) + (blockX * 8) + pixelX) * 4;

					imageData[destIndex] = data;
					imageData[destIndex + 1] = data;
					imageData[destIndex + 2] = data;
					imageData[destIndex + 3] = data;
				}
			}
		}
	}
}

void J3DTextureFactory::DecodeIA4(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry) {
	if (imageData == nullptr)
		return;

	uint32_t numBlocksW = entry.Width / 8;
	uint32_t numBlocksH = entry.Height / 4;

	// Iterate the blocks in the image
	for (int blockY = 0; blockY < numBlocksH; blockY++) {
		for (int blockX = 0; blockX < numBlocksW; blockX++) {
			// Iterate the pixels in the current block
			for (int pixelY = 0; pixelY < 4; pixelY++) {
				for (int pixelX = 0; pixelX < 8; pixelX++) {
					// Bounds check to ensure the pixel is within the image.
					if ((blockX * 8 + pixelX >= entry.Width) || (blockY * 4 + pixelY >= entry.Height))
						continue;

					uint8_t data = stream->readUInt8();

					// Each byte contains alpha and luminance of the current pixel.
					uint8_t alpha = (data & 0xF0) >> 4;
					uint8_t luminance = (data & 0x0F);

					uint32_t destIndex = (entry.Width * ((blockY * 4) + pixelY) + (blockX * 8) + pixelX) * 4;

					imageData[destIndex] = luminance * 0x11;
					imageData[destIndex + 1] = luminance * 0x11;
					imageData[destIndex + 2] = luminance * 0x11;
					imageData[destIndex + 3] = alpha * 0x11;
				}
			}
		}
	}
}

void J3DTextureFactory::DecodeIA8(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry) {
	if (imageData == nullptr)
		return;

	uint32_t numBlocksW = entry.Width / 4;
	uint32_t numBlocksH = entry.Height / 4;

	// Iterate the blocks in the image
	for (int blockY = 0; blockY < numBlocksH; blockY++) {
		for (int blockX = 0; blockX < numBlocksW; blockX++) {
			// Iterate the pixels in the current block
			for (int pixelY = 0; pixelY < 4; pixelY++) {
				for (int pixelX = 0; pixelX < 4; pixelX++) {
					// Bounds check to ensure the pixel is within the image.
					if ((blockX * 4 + pixelX >= entry.Width) || (blockY * 4 + pixelY >= entry.Height))
						continue;

					// The alpha and luminance values of the current pixel are stored in two bytes.
					uint8_t alpha = stream->readUInt8();
					uint8_t luminance = stream->readUInt8();

					uint32_t destIndex = (entry.Width * ((blockY * 4) + pixelY) + (blockX * 4) + pixelX) * 4;

					imageData[destIndex] = luminance;
					imageData[destIndex + 1] = luminance;
					imageData[destIndex + 2] = luminance;
					imageData[destIndex + 3] = alpha;
				}
			}
		}
	}
}

void J3DTextureFactory::DecodeRGB565(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry) {
	if (imageData == nullptr)
		return;

	uint32_t numBlocksW = entry.Width / 4;
	uint32_t numBlocksH = entry.Height / 4;

	// Iterate the blocks in the image
	for (int blockY = 0; blockY < numBlocksH; blockY++) {
		for (int blockX = 0; blockX < numBlocksW; blockX++) {
			// Iterate the pixels in the current block
			for (int pixelY = 0; pixelY < 4; pixelY++) {
				for (int pixelX = 0; pixelX < 4; pixelX++) {
					// Bounds check to ensure the pixel is within the image.
					if ((blockX * 4 + pixelX >= entry.Width) || (blockY * 4 + pixelY >= entry.Height))
						continue;

					// RGB values for this pixel are stored in a 16-bit integer.
					uint16_t data = stream->readUInt16();
					uint32_t rgba8 = RGB565toRGBA8(data);

					uint32_t destIndex = (entry.Width * ((blockY * 4) + pixelY) + (blockX * 4) + pixelX) * 4;

					imageData[destIndex] = (rgba8 & 0xFF000000) >> 24;
					imageData[destIndex + 1] = (rgba8 & 0x00FF0000) >> 16;
					imageData[destIndex + 2] = (rgba8 & 0x0000FF00) >> 8;
					imageData[destIndex + 3] = rgba8 & 0x000000FF;
				}
			}
		}
	}
}

void J3DTextureFactory::DecodeRGB5A3(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry) {
	if (imageData == nullptr)
		return;

	uint32_t numBlocksW = entry.Width / 4;
	uint32_t numBlocksH = entry.Height / 4;

	// Iterate the blocks in the image
	for (int blockY = 0; blockY < numBlocksH; blockY++) {
		for (int blockX = 0; blockX < numBlocksW; blockX++) {
			// Iterate the pixels in the current block
			for (int pixelY = 0; pixelY < 4; pixelY++) {
				for (int pixelX = 0; pixelX < 4; pixelX++) {
					// Bounds check to ensure the pixel is within the image.
					if ((blockX * 4 + pixelX >= entry.Width) || (blockY * 4 + pixelY >= entry.Height))
						continue;

					// RGB values for this pixel are stored in a 16-bit integer.
					uint16_t data = stream->readUInt16();
					uint32_t rgba8 = RGB5A3toRGBA8(data);

					uint32_t destIndex = (entry.Width * ((blockY * 4) + pixelY) + (blockX * 4) + pixelX) * 4;

					imageData[destIndex] = (rgba8 & 0xFF000000) >> 24;
					imageData[destIndex + 1] = (rgba8 & 0x00FF0000) >> 16;
					imageData[destIndex + 2] = (rgba8 & 0x0000FF00) >> 8;
					imageData[destIndex + 3] = rgba8 & 0x000000FF;
				}
			}
		}
	}
}

void J3DTextureFactory::DecodeRGBA32(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry) {
	if (imageData == nullptr)
		return;

	uint32_t numBlocksW = entry.Width / 4;
	uint32_t numBlocksH = entry.Height / 4;

	// Iterate the blocks in the image
	for (int blockY = 0; blockY < numBlocksH; blockY++) {
		for (int blockX = 0; blockX < numBlocksW; blockX++) {
			// Iterate the pixels in the current block
			
			// Alpha/red values for current pixel
			for (int pixelY = 0; pixelY < 4; pixelY++) {
				for (int pixelX = 0; pixelX < 4; pixelX++) {
					// Bounds check to ensure the pixel is within the image.
					if ((blockX * 4 + pixelX >= entry.Width) || (blockY * 4 + pixelY >= entry.Height))
						continue;

					uint32_t destIndex = (entry.Width * ((blockY * 4) + pixelY) + (blockX * 4) + pixelX) * 4;

					imageData[destIndex + 3] = stream->readUInt8();
					imageData[destIndex + 2] = stream->readUInt8();
				}
			}

			// Green/blue values for current pixel
			for (int pixelY = 0; pixelY < 4; pixelY++) {
				for (int pixelX = 0; pixelX < 4; pixelX++) {
					// Bounds check to ensure the pixel is within the image.
					if ((blockX * 4 + pixelX >= entry.Width) || (blockY * 4 + pixelY >= entry.Height))
						continue;

					uint32_t destIndex = (entry.Width * ((blockY * 4) + pixelY) + (blockX * 4) + pixelX) * 4;

					imageData[destIndex + 1] = stream->readUInt8();
					imageData[destIndex] = stream->readUInt8();
				}
			}
		}
	}
}

void J3DTextureFactory::DecodeC4(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry, uint8_t* palette) {
	if (imageData == nullptr || palette == nullptr)
		return;

	uint8_t* buffer = new uint8_t[entry.Width * entry.Height * 8];

	uint32_t numBlocksW = entry.Width / 8;
	uint32_t numBlocksH = entry.Height / 8;

	// Iterate the blocks in the image
	for (int blockY = 0; blockY < numBlocksH; blockY++) {
		for (int blockX = 0; blockX < numBlocksW; blockX++) {
			// Iterate the pixels in the current block
			for (int pixelY = 0; pixelY < 8; pixelY++) {
				for (int pixelX = 0; pixelX < 8; pixelX += 2) {
					// Bounds check to ensure the pixel is within the image.
					if ((blockX * 8 + pixelX >= entry.Width) || (blockY * 8 + pixelY >= entry.Height))
						continue;

					uint8_t data = stream->readUInt8();

					// Each byte contains the palette indices for 2 pixels.
					uint8_t pixel0 = (data & 0xF0) >> 4;
					uint8_t pixel1 = (data & 0x0F);

					uint32_t destIndex = (entry.Width * ((blockY * 8) + pixelY) + (blockX * 8) + pixelX);

					buffer[destIndex] = pixel0;
					buffer[destIndex + 1] = pixel1;
				}
			}
		}
	}

	uint32_t pixelSize = entry.PaletteFormat == EGXPaletteFormat::IA8 ? 2 : 4;
	uint32_t destOffset = 0;
	for (int y = 0; y < entry.Height; y++) {
		for (int x = 0; x < entry.Width; x++) {
			UnpackPixelFromPalette(palette, buffer[y * entry.Width + x], imageData, destOffset, entry.PaletteFormat);
			destOffset += pixelSize;
		}
	}

	delete[] buffer;
}

void J3DTextureFactory::DecodeC8(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry, uint8_t* palette) {
	if (imageData == nullptr || palette == nullptr)
		return;

	uint8_t* buffer = new uint8_t[entry.Width * entry.Height * 8];

	uint32_t numBlocksW = entry.Width / 8;
	uint32_t numBlocksH = entry.Height / 4;

	// Iterate the blocks in the image
	for (int blockY = 0; blockY < numBlocksH; blockY++) {
		for (int blockX = 0; blockX < numBlocksW; blockX++) {
			// Iterate the pixels in the current block
			for (int pixelY = 0; pixelY < 4; pixelY++) {
				for (int pixelX = 0; pixelX < 8; pixelX++) {
					// Bounds check to ensure the pixel is within the image.
					if ((blockX * 8 + pixelX >= entry.Width) || (blockY * 4 + pixelY >= entry.Height))
						continue;

					uint8_t data = stream->readUInt8();
					uint32_t destIndex = (entry.Width * ((blockY * 4) + pixelY) + (blockX * 8) + pixelX);

					buffer[destIndex] = data;
				}
			}
		}
	}

	uint32_t pixelSize = entry.PaletteFormat == EGXPaletteFormat::IA8 ? 2 : 4;
	uint32_t destOffset = 0;
	for (int y = 0; y < entry.Height; y++) {
		for (int x = 0; x < entry.Width; x++) {
			UnpackPixelFromPalette(palette, buffer[y * entry.Width + x], imageData, destOffset, entry.PaletteFormat);
			destOffset += pixelSize;
		}
	}

	delete[] buffer;
}

void J3DTextureFactory::DecodeCMPR(bStream::CStream* stream, uint8_t* imageData, J3DTextureEntry& entry) {

}

void J3DTextureFactory::UnpackPixelFromPalette(uint8_t* palette, uint32_t index, uint8_t* dest, uint32_t offset, EGXPaletteFormat format) {
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

uint32_t J3DTextureFactory::RGB565toRGBA8(uint16_t data) {
	uint8_t r = (data & 0xF100) >> 11;
	uint8_t g = (data & 0x07E0) >> 5;
	uint8_t b = (data & 0x001F);

	uint32_t output = 0x000000FF;
	output |= (r << 3) << 24;
	output |= (g << 2) << 16;
	output |= (b << 3) << 8;

	return output;
}

uint32_t J3DTextureFactory::RGB5A3toRGBA8(uint16_t data) {
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
