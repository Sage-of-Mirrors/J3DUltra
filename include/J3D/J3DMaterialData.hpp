#pragma once

#include "GX/GXEnum.hpp"
#include <cstdint>
#include <glm/vec4.hpp>

namespace bStream { class CStream; }

enum class EPixelEngineMode : uint8_t {
	Opaque = 1,
	AlphaTest = 2,
	Translucent = 4
};

struct J3DMaterialComponentBase {
	virtual void Deserialize(bStream::CStream* stream) = 0;
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
};

struct J3DBlendMode : public J3DMaterialComponentBase {
	EGXBlendMode Type = EGXBlendMode::None;
	EGXBlendModeControl SourceFactor = EGXBlendModeControl::One;
	EGXBlendModeControl DestinationFactor = EGXBlendModeControl::Zero;
	EGXLogicOp Operation = EGXLogicOp::Copy;

	virtual void Deserialize(bStream::CStream* stream);
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
};
