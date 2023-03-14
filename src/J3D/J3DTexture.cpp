#include "J3D/J3DTexture.hpp"

#include <bstream.h>

J3DTexture::J3DTexture() {

}

J3DTexture::~J3DTexture() {
	for (uint8_t* img : ImageData) {
		delete[] img;
	}
}

void J3DTexture::Deserialize(bStream::CStream* stream) {
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
	MaxAnisotropy = (EGXMaxAnisotropy)stream->readUInt8();
	MinFilter = (EGXFilterMode)stream->readUInt8();
	MagFilter = (EGXFilterMode)stream->readUInt8();

	MinLOD = stream->readUInt8();
	MaxLOD = stream->readUInt8();
	MipmapCount = stream->readUInt8();
	Unknown = stream->readUInt8();
	LODBias = stream->readUInt16();
	TextureOffset = stream->readUInt32();
}
