#pragma once

#include <cstdint>

/*enum class EGXAttribute {
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
};*/

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

enum class EGXLogicOp : uint8_t {
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

enum class EGXFogType : uint8_t {
	None,
	Linear,
	Exponential,
	Exponential_2,
	Reverse_Exponential,
	Reverse_Exponential_2
};

enum class EGXLightMask : uint8_t {
	Light0 = 0x01,
	Light1 = 0x02,
	Light2 = 0x04,
	Light3 = 0x08,
	Light4 = 0x10,
	Light5 = 0x20,
	Light6 = 0x40,
	Light7 = 0x80,
	None = 0x00
};

enum class EGXDiffuseFunction : uint8_t {
	None,
	Signed,
	Clamp
};

enum class EGXAttenuationFunction : uint8_t {
	Spec = 0,	// Specular Computation
	Spot = 1,	// Spot Light Attenuation
	None = 2, 	// No attenuation
};

enum class EGXColorSource : uint8_t {
	Register, // Use Register Colors
	Vertex    // Use Vertex Colors
};

enum class EGXTexGenSrc : uint8_t {
	Position = 0,
	Normal = 1,
	Binormal = 2,
	Tangent = 3,
	Tex0 = 4,
	Tex1 = 5,
	Tex2 = 6,
	Tex3 = 7,
	Tex4 = 8,
	Tex5 = 9,
	Tex6 = 10,
	Tex7 = 11,
	TexCoord0 = 12,
	TexCoord1 = 13,
	TexCoord2 = 14,
	TexCoord3 = 15,
	TexCoord4 = 16,
	TexCoord5 = 17,
	TexCoord6 = 18,
	Color0 = 19,
	Color1 = 20,
};

enum class EGXTexGenType : uint8_t {
	Matrix3x4 = 0,
	Matrix2x4 = 1,
	Bump0 = 2,
	Bump1 = 3,
	Bump2 = 4,
	Bump3 = 5,
	Bump4 = 6,
	Bump5 = 7,
	Bump6 = 8,
	Bump7 = 9,
	SRTG = 10
};

enum class EGXTexMatrix : uint8_t {
	TexMtx0 = 30,
	TexMtx1 = 33,
	TexMtx2 = 36,
	TexMtx3 = 39,
	TexMtx4 = 42,
	TexMtx5 = 45,
	TexMtx6 = 48,
	TexMtx7 = 51,
	TexMtx8 = 54,
	TexMtx9 = 57,
	Identity = 60,
};

enum class EGXTexMatrixType : uint8_t {
	Matrix3x4,
	Matrix2x4
};

enum class EGXTexCoordSlot : uint8_t {
	TexCoord0 = 0,
	TexCoord1 = 1,
	TexCoord2 = 2,
	TexCoord3 = 3,
	TexCoord4 = 4,
	TexCoord5 = 5,
	TexCoord6 = 6,
	TexCoord7 = 7,
	Null = 0xFF
};

enum class EGXColorChannelId : uint8_t {
	Color0 = 0,
	Alpha0 = 1,
	Color1 = 2,
	Alpha1 = 3,
	Color0A0 = 4,
	Color1A1 = 5,
	ColorZero = 6,
	AlphaBump = 7,
	AlphaBumpN = 8,
	ColorNull = 0xFF,
};

enum class EGXTevRegister : uint8_t {
	Prev = 0,
	Reg0 = 1,
	Reg1 = 2,
	Reg2 = 3,
};

enum class EGXCombineColorInput : uint8_t {
	ColorPrev = 0,  // ! < Use Color Value from previous TEV stage
	AlphaPrev = 1,  // ! < Use Alpha Value from previous TEV stage
	C0 = 2,         // ! < Use the Color Value from the Color/Output Register 0
	A0 = 3,         // ! < Use the Alpha value from the Color/Output Register 0
	C1 = 4,         // ! < Use the Color Value from the Color/Output Register 1
	A1 = 5,         // ! < Use the Alpha value from the Color/Output Register 1
	C2 = 6,         // ! < Use the Color Value from the Color/Output Register 2
	A2 = 7,         // ! < Use the Alpha value from the Color/Output Register 2
	TexColor = 8,   // ! < Use the Color value from Texture
	TexAlpha = 9,   // ! < Use the Alpha value from Texture
	RasColor = 10,  // ! < Use the color value from rasterizer
	RasAlpha = 11,  // ! < Use the alpha value from rasterizer
	One = 12,
	Half = 13,
	Konst = 14, // ToDo: Is this GX_CC_QUARTER?
	Zero = 15       // 
					// ToDo: Is this missing GX_CC_TEXRRR, GX_CC_TEXGGG, GX_CC_TEXBBBB?
};

enum class EGXCombineAlphaInput : uint8_t {
	AlphaPrev = 0,  // Use the Alpha value form the previous TEV stage
	A0 = 1,         // Use the Alpha value from the Color/Output Register 0
	A1 = 2,         // Use the Alpha value from the Color/Output Register 1
	A2 = 3,         // Use the Alpha value from the Color/Output Register 2
	TexAlpha = 4,   // Use the Alpha value from the Texture
	RasAlpha = 5,   // Use the Alpha value from the rasterizer
	Konst = 6,      // ToDO: Is this GX_CA_ONE?
	Zero = 7
};

enum class EGXTevOp : uint8_t {
	Add = 0,
	Sub = 1,
	Comp_R8_GT = 8,
	Comp_R8_EQ = 9,
	Comp_GR16_GT = 10,
	Comp_GR16_EQ = 11,
	Comp_BGR24_GT = 12,
	Comp_BGR24_EQ = 13,
	Comp_RGB8_GT = 14,
	Comp_RGB8_EQ = 15,
	Comp_A8_GT = Comp_RGB8_GT,
	Comp_A8_EQ = Comp_RGB8_EQ
};

enum class EGXTevBias : uint8_t {
	Zero = 0,
	AddHalf = 1,
	SubHalf = 2
};

enum class EGXTevScale : uint8_t {
	Scale_1 = 0,
	Scale_2 = 1,
	Scale_4 = 2,
	Divide_2 = 3
};

enum class EGXKonstColorSel : uint8_t {
	KCSel_1 = 0x00,     // Constant 1.0
	KCSel_7_8 = 0x01,   // Constant 7/8
	KCSel_3_4 = 0x02,   // Constant 3/4
	KCSel_5_8 = 0x03,   // Constant 5/8
	KCSel_1_2 = 0x04,   // Constant 1/2
	KCSel_3_8 = 0x05,   // Constant 3/8
	KCSel_1_4 = 0x06,   // Constant 1/4
	KCSel_1_8 = 0x07,   // Constant 1/8
	KCSel_K0 = 0x0C,    // K0[RGB] Register
	KCSel_K1 = 0x0D,    // K1[RGB] Register
	KCSel_K2 = 0x0E,    // K2[RGB] Register
	KCSel_K3 = 0x0F,    // K3[RGB] Register
	KCSel_K0_R = 0x10,  // K0[RRR] Register
	KCSel_K1_R = 0x11,  // K1[RRR] Register
	KCSel_K2_R = 0x12,  // K2[RRR] Register
	KCSel_K3_R = 0x13,  // K3[RRR] Register
	KCSel_K0_G = 0x14,  // K0[GGG] Register
	KCSel_K1_G = 0x15,  // K1[GGG] Register
	KCSel_K2_G = 0x16,  // K2[GGG] Register
	KCSel_K3_G = 0x17,  // K3[GGG] Register
	KCSel_K0_B = 0x18,  // K0[BBB] Register
	KCSel_K1_B = 0x19,  // K1[BBB] Register
	KCSel_K2_B = 0x1A,  // K2[BBB] Register
	KCSel_K3_B = 0x1B,  // K3[BBB] Register
	KCSel_K0_A = 0x1C,  // K0[AAA] Register
	KCSel_K1_A = 0x1D,  // K1[AAA] Register
	KCSel_K2_A = 0x1E,  // K2[AAA] Register
	KCSel_K3_A = 0x1F   // K3[AAA] Register
};

enum class EGXKonstAlphaSel : uint8_t {
	KASel_1 = 0x00,     // Constant 1.0
	KASel_7_8 = 0x01,   // Constant 7/8
	KASel_3_4 = 0x02,   // Constant 3/4
	KASel_5_8 = 0x03,   // Constant 5/8
	KASel_1_2 = 0x04,   // Constant 1/2
	KASel_3_8 = 0x05,   // Constant 3/8
	KASel_1_4 = 0x06,   // Constant 1/4
	KASel_1_8 = 0x07,   // Constant 1/8
	KASel_K0_R = 0x10,  // K0[R] Register
	KASel_K1_R = 0x11,  // K1[R] Register
	KASel_K2_R = 0x12,  // K2[R] Register
	KASel_K3_R = 0x13,  // K3[R] Register
	KASel_K0_G = 0x14,  // K0[G] Register
	KASel_K1_G = 0x15,  // K1[G] Register
	KASel_K2_G = 0x16,  // K2[G] Register
	KASel_K3_G = 0x17,  // K3[G] Register
	KASel_K0_B = 0x18,  // K0[B] Register
	KASel_K1_B = 0x19,  // K1[B] Register
	KASel_K2_B = 0x1A,  // K2[B] Register
	KASel_K3_B = 0x1B,  // K3[B] Register
	KASel_K0_A = 0x1C,  // K0[A] Register
	KASel_K1_A = 0x1D,  // K1[A] Register
	KASel_K2_A = 0x1E,  // K2[A] Register
	KASel_K3_A = 0x1F   // K3[A] Register
};

enum class EGXSwapMode : uint8_t {
	R,
	G,
	B,
	A
};

enum class EGXTextureFormat : uint32_t {
	I4,     // 4BPP, 8x8 blocks, monochrome
	I8,     // 8BPP, 8x8 blocks, monochrome
	IA4,    // 8BPP, 8x4 blocks, monochrome + alpha
	IA8,    // 16BPP, 4x4 blocks, monochrome + alpha
	RGB565, // 16BPP, 4x4 blocks, color
	RGB5A3, // 16BPP, 4x4 blocks, color + alpha
	RGBA32, // 32BPP, 4x4 blocks, color + alpha

	// There is no enum value associated with ID 0x07.

	C4 = 8, // 4BPP, 8x8 blocks, palette
	C8,     // 8BPP, 8x4 blocks, palette
	C14X2,  // 16BPP, 4x4 blocks, palette

	// There are no enum values associated with IDs 0x0B to 0x0D.

	CMPR = 14    // 4BPP, 8x8 blocks, palette per block
};

enum class EGXWrapMode : uint32_t {
	ClampToEdge,
	Repeat,
	MirroredRepeat
};

enum class EGXPaletteFormat : uint32_t {
	IA8,
	RGB565,
	RGB5A3
};

enum class EGXFilterMode : uint32_t {
	// Valid in both Min and Mag Filter
	Nearest,                  // Point Sampling, No Mipmap
	Linear,                   // Bilinear Filtering, No Mipmap

	// Valid in only Min Filter
	NearestMipmapNearest,     // Point Sampling, Discrete Mipmap
	NearestMipmapLinear,      // Bilinear Filtering, Discrete Mipmap
	LinearMipmapNearest,      // Point Sampling, Linear MipMap
	LinearMipmapLinear,       // Trilinear Filtering
};
