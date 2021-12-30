#pragma once

#include <cstdint>

enum class EGXAttribute {
	PositionMatrixIdx,
	Tex0MatrixIdx,
	Tex1MatrixIdx,
	Tex2MatrixIdx,
	Tex3MatrixIdx,
	Tex4MatrixIdx,
	Tex5MatrixIdx,
	Tex6MatrixIdx,
	Tex7MatrixIdx,

	Position,
	Normal,
	Color0,
	Color1,
	TexCoord0,
	TexCoord1,
	TexCoord2,
	TexCoord3,
	TexCoord4,
	TexCoord5,
	TexCoord6,
	TexCoord7,

	PositionMatrixArray,
	NormalMatrixArray,
	TexMatrixArray,
	LightArray,
	NBT,

	Null = 0xFF
};

enum class EGXComponentCount : uint32_t {
	Position_XY = 0,
	Position_XYZ,

	Normal_XYZ = 0,
	Normal_NBT,
	Normal_NBT3,

	Color_RGB = 0,
	Color_RGBA,

	TexCoord_U = 0,
	TexCoord_UV
};

enum class EGXComponentType : uint32_t {
	Unsigned8,
	Signed8,
	Unsigned16,
	Signed16,
	Float,

	// Colors
	RGB565 = 0,
	RGB8,
	RGBX8,
	RGBA4,
	RGBA6,
	RGBA8
};

enum class EGXPrimitiveType : uint8_t {
	None = 0,

	Quads = 0x80,
	Triangles = 0x90,
	TriangleStrips = 0x98,
	TriangleFan = 0xA0,
	Lines = 0xA8,
	LineStrips = 0xB0,
	Points = 0xB8
};
