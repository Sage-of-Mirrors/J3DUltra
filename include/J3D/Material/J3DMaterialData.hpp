#pragma once

#include "GX/GXEnum.hpp"
#include "J3D/Util/J3DTransform.hpp"
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
	virtual void Serialize(bStream::CStream* stream) = 0;
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

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream) override;
	virtual size_t GetElementSize() override { return 4; }

	bool operator==(const J3DZMode& other) const;
	bool operator!=(const J3DZMode& other) const;
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

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 8; }

	bool operator==(const J3DAlphaCompare& other) const;
	bool operator!=(const J3DAlphaCompare& other) const;
};

struct J3DBlendMode : public J3DMaterialComponentBase {
	EGXBlendMode Type = EGXBlendMode::None;
	EGXBlendModeControl SourceFactor = EGXBlendModeControl::One;
	EGXBlendModeControl DestinationFactor = EGXBlendModeControl::Zero;
	EGXLogicOp Operation = EGXLogicOp::Copy;

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 4; }

	bool operator==(const J3DBlendMode& other) const;
	bool operator!=(const J3DBlendMode& other) const;
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

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 44; }

	bool operator==(const J3DFog& other) const;
	bool operator!=(const J3DFog& other) const;
};

struct J3DColorChannel : public J3DMaterialComponentBase {
	bool LightingEnabled;
	EGXColorSource MaterialSource;
	uint8_t LightMask;
	EGXDiffuseFunction DiffuseFunction;
	EGXAttenuationFunction AttenuationFunction;
	EGXColorSource AmbientSource;

	J3DColorChannel();

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 8; }

	bool operator==(const J3DColorChannel& other) const;
	bool operator!=(const J3DColorChannel& other) const;
};

struct J3DTexCoordInfo : public J3DMaterialComponentBase {
	EGXTexGenType Type;
	EGXTexGenSrc Source;
	EGXTexMatrix TexMatrix;

	J3DTexCoordInfo();

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 4; }

	bool operator==(const J3DTexCoordInfo& other) const;
	bool operator!=(const J3DTexCoordInfo& other) const;
};

enum class EJ3DMatrixCalcType : uint8_t {
	SOFTIMAGE,
	MAYA
};

enum class EJ3DTexEffect : uint8_t {
	NONE,
	ENVMAP_BASIC,
	PROJMAP_BASIC,
	VIEWPROJMAP_BASIC,
	EFFECT_4,
	EFFECT_5,
	ENVMAP_OLD,
	ENVMAP,
	PROJMAP,
	VIEWPROJMAP,
	ENVMAP_OLD_EFFECTMTX,
	ENVMAP_EFFECTMTX
};

struct J3DTexMatrixInfo : public J3DMaterialComponentBase {
	EGXTexMatrixType Type;
	EJ3DMatrixCalcType CalcType;
	EJ3DTexEffect TexEffect;

	glm::vec3 Origin;
	J3DTextureSRTInfo Transform;
	glm::mat4 ProjectionMatrix;
	
	glm::mat4 CalculatedMatrix;

	J3DTexMatrixInfo();

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 100; }

	void CalculateMatrix();

	bool operator==(const J3DTexMatrixInfo& other) const;
	bool operator!=(const J3DTexMatrixInfo& other) const;
};

struct J3DNBTScaleInfo : public J3DMaterialComponentBase {
	bool Enable;
	glm::vec3 Scale;

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 16; }

	bool operator==(const J3DNBTScaleInfo& other) const;
	bool operator!=(const J3DNBTScaleInfo& other) const;
};

struct J3DSwapModeInfo : public J3DMaterialComponentBase {
	uint8_t RasIndex, TexIndex;

	J3DSwapModeInfo();
	J3DSwapModeInfo(uint8_t ras, uint8_t tex);

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 4; }

	bool operator==(const J3DSwapModeInfo& other) const;
	bool operator!=(const J3DSwapModeInfo& other) const;
};

struct J3DSwapModeTableInfo : public J3DMaterialComponentBase {
	EGXSwapMode R, G, B, A;

	J3DSwapModeTableInfo();

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 4; }

	bool operator==(const J3DSwapModeTableInfo& other) const;
	bool operator!=(const J3DSwapModeTableInfo& other) const;
};

struct J3DTevOrderInfo : public J3DMaterialComponentBase {
	EGXTexCoordSlot TexCoordId;
	EGXTexMapSlot TexMapId;
	EGXColorChannelId ChannelId;

	J3DSwapModeTableInfo mTexSwapMode;
	J3DSwapModeTableInfo mRasSwapMode;

	J3DTevOrderInfo();

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 4; }

	bool operator==(const J3DTevOrderInfo& other) const;
	bool operator!=(const J3DTevOrderInfo& other) const;
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

	J3DTevStageInfo();

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 20; }

	bool operator==(const J3DTevStageInfo& other) const;
	bool operator!=(const J3DTevStageInfo& other) const;
};

struct J3DIndirectTexMatrixInfo : public J3DMaterialComponentBase {
	glm::mat2x3 TexMatrix;
	uint8_t Exponent;

	J3DIndirectTexMatrixInfo();

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 20; }

	bool operator==(const J3DIndirectTexMatrixInfo& other) const;
	bool operator!=(const J3DIndirectTexMatrixInfo& other) const;
};

struct J3DIndirectTexScaleInfo : public J3DMaterialComponentBase {
	EGXIndirectTexScale ScaleS;
	EGXIndirectTexScale ScaleT;

	J3DIndirectTexScaleInfo();

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 20; }

	bool operator==(const J3DIndirectTexScaleInfo& other) const;
	bool operator!=(const J3DIndirectTexScaleInfo& other) const;
};

struct J3DIndirectTexOrderInfo : public J3DMaterialComponentBase {
	EGXTexCoordSlot TexCoordId;
	EGXTexMapSlot TexMapId;

	J3DIndirectTexOrderInfo();

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 20; }

	bool operator==(const J3DIndirectTexOrderInfo& other) const;
	bool operator!=(const J3DIndirectTexOrderInfo& other) const;
};

struct J3DIndirectTevStageInfo : public J3DMaterialComponentBase {
	EGXTevStageId TevStageId;
	EGXIndirectTexFormat TexFormat;
	EGXIndirectTexBias TexBias;
	EGXIndirectTexMatrixId TexMtxId;
	EGXIndirectWrapMode TexWrapS;
	EGXIndirectWrapMode TexWrapT;
	bool AddPrev;
	bool UtcLod;
	EGXIndirectAlphaSel AlphaSel;

	J3DIndirectTevStageInfo();

	virtual void Serialize(bStream::CStream* stream) override;
	virtual void Deserialize(bStream::CStream* stream);
	virtual size_t GetElementSize() override { return 20; }

	bool operator==(const J3DIndirectTevStageInfo& other) const;
	bool operator!=(const J3DIndirectTevStageInfo& other) const;
};
