#pragma once

#include "GX/GXEnum.hpp"
#include "J3D/J3DTransform.hpp"
#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace bStream { class CStream; }

enum class EPixelEngineMode : uint8_t {
	Opaque = 1,
	AlphaTest = 2,
	Translucent = 4
};

struct J3DMaterialComponentBase {
	virtual void Deserialize(bStream::CStream* stream) = 0;
	virtual size_t GetElementSize() = 0;
};

// Z-buffer settings.
struct J3DZMode : public J3DMaterialComponentBase {
	// Enable or disable the Z buffer.
	bool Enable = false;
	// Function comparing new Z value and the buffered Z value to determine which to discard.
	EGXCompareType Function = EGXCompareType::Never;
	// Enable or disable updates to the Z buffer.
	bool UpdateEnable = false;

	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 4; }
};

// Alpha compare settings. The formula is:
// alpha_pass = (src <CompareFunc0> Reference0) op (src <CompareFunc1> Reference1)
// where src is the alpha value from the last active TEV stage.
struct J3DAlphaCompare : public J3DMaterialComponentBase {
	EGXCompareType CompareFunc0 = EGXCompareType::Never;
	uint8_t Reference0 = 0;

	EGXAlphaOp Operation = EGXAlphaOp::And;

	EGXCompareType CompareFunc1 = EGXCompareType::Never;
	uint8_t Reference1 = 0;

	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 8; }
};

struct J3DBlendMode : public J3DMaterialComponentBase {
	EGXBlendMode Type = EGXBlendMode::None;
	EGXBlendModeControl SourceFactor = EGXBlendModeControl::One;
	EGXBlendModeControl DestinationFactor = EGXBlendModeControl::Zero;
	EGXLogicOp Operation = EGXLogicOp::Copy;

	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 4; }
};

struct J3DFog : public J3DMaterialComponentBase {
	EGXFogType Type;
	bool Enable;
	uint16_t Center;
	float StartZ;
	float EndZ;
	float FarZ;
	float NearZ;
	glm::vec4 Color;

	uint16_t AdjustmentTable[10];

	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 44; }
};

struct J3DColorChannel : public J3DMaterialComponentBase {
	bool LightingEnabled;
	EGXColorSource MaterialSource;
	uint8_t LightMask;
	EGXDiffuseFunction DiffuseFunction;
	EGXAttenuationFunction AttenuationFunction;
	EGXColorSource AmbientSource;

	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 8; }
};

struct J3DTexCoordInfo : public J3DMaterialComponentBase {
	EGXTexGenType Type;
	EGXTexGenSrc Source;
	EGXTexMatrix TexMatrix;

	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 4; }
};

enum class EJ3DTexMatrixProjection : uint8_t {
	ST,
	STQ
};

struct J3DTexMatrixInfo : public J3DMaterialComponentBase {
	EJ3DTexMatrixProjection Projection;
	uint8_t Type;
	glm::vec3 Origin;

	J3DTextureSRTInfo Transform;
	glm::mat4 Matrix;

	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 100; }
};

struct J3DNBTScaleInfo : public J3DMaterialComponentBase {
	bool Enable;
	glm::vec3 Scale;

	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 16; }
};

struct J3DTevOrderInfo : public J3DMaterialComponentBase {
	EGXTexCoordSlot TexCoordId;
	uint8_t TexMap;
	EGXColorChannelId ChannelId;

	uint8_t mTexSwapTable[4]{};
	uint8_t mRasSwapTable[4]{};

	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 4; }
};

struct J3DTevStageInfo : public J3DMaterialComponentBase {
	uint8_t Unknown0;

	EGXCombineColorInput ColorInput[4];
	EGXTevOp ColorOperation;
	EGXTevBias ColorBias;
	EGXTevScale ColorScale;
	bool ColorClamp;
	EGXTevRegister ColorOutputRegister;

	EGXCombineAlphaInput AlphaInput[4];
	EGXTevOp AlphaOperation;
	EGXTevBias AlphaBias;
	EGXTevScale AlphaScale;
	bool AlphaClamp;
	EGXTevRegister AlphaOutputRegister;

	uint8_t Unknown1;

	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 20; }
};
