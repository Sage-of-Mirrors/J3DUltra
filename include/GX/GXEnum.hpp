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

enum class EGXAttributeType : uint32_t {
	None,
	Direct,
	Index8,
	Index16
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

enum class EGXCullMode : uint8_t {
	None,
	Front,
	Back,
	All
};

enum class EGXCompareType : uint8_t {
	Never,
	Less,
	Equal,
	LEqual,
	Greater,
	NEqual,
	GEqual,
	Always
};

enum class EGXAlphaOp : uint8_t {
	And,
	Or,
	XOR,
	XNOR
};

enum class EGXBlendMode : uint8_t {
	None,
	Blend,
	Logic,
	Subtract
};

enum class EGXBlendModeControl : uint8_t {
	Zero,               // ! < 0.0
	One,                // ! < 1.0
	SrcColor,           // ! < Source Color
	InverseSrcColor,    // ! < 1.0 - (Source Color)
	SrcAlpha,           // ! < Source Alpha
	InverseSrcAlpha,    // ! < 1.0 - (Source Alpha)
	DstAlpha,           // ! < Framebuffer Alpha
	InverseDstAlpha     // ! < 1.0 - (Framebuffer Alpha)
};

enum class EGXLogicOp {
	Clear = 0,
	And = 1,
	Copy = 3,
	Equiv = 9,
	Inv = 10,
	InvAnd = 4,
	InvCopy = 12,
	InvOr = 13,
	NAnd = 14,
	NoOp = 5,
	NOr = 8,
	Or = 7,
	RevAnd = 2,
	RevOr = 11,
	Set = 15,
	XOr = 6
};

enum class EGXFogType {
	None,
	Linear,
	Exponential,
	Exponential_2,
	Reverse_Exponential,
	Reverse_Exponential_2
};
