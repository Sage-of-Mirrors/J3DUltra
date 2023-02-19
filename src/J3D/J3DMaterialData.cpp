#include "J3D/J3DMaterialData.hpp"
#include <bstream.h>

/* == J3DZMode == */
void J3DZMode::Deserialize(bStream::CStream* stream) {
	Enable = stream->readUInt8();
	Function = (EGXCompareType)stream->readUInt8();
	UpdateEnable = stream->readUInt8();
}

bool J3DZMode::operator==(const J3DZMode& other) const {
	return Enable == other.Enable && Function == other.Function && UpdateEnable == other.UpdateEnable;
}

bool J3DZMode::operator!=(const J3DZMode& other) const {
	return !operator==(other);
}

/* == J3DAlphaCompare == */
void J3DAlphaCompare::Deserialize(bStream::CStream* stream) {
	CompareFunc0 = (EGXCompareType)stream->readUInt8();
	Reference0 = stream->readUInt8();

	Operation = (EGXAlphaOp)stream->readUInt8();

	CompareFunc1 = (EGXCompareType)stream->readUInt8();
	Reference1 = stream->readUInt8();
}

bool J3DAlphaCompare::operator==(const J3DAlphaCompare& other) const {
	return CompareFunc0 == other.CompareFunc0 && Reference0 == other.Reference0 && Operation == other.Operation &&
		CompareFunc1 == other.CompareFunc1 && Reference1 == other.Reference1;
}

bool J3DAlphaCompare::operator!=(const J3DAlphaCompare& other) const {
	return !operator==(other);
}

/* == J3DBlendMode == */
void J3DBlendMode::Deserialize(bStream::CStream* stream) {
	Type = (EGXBlendMode)stream->readUInt8();
	SourceFactor = (EGXBlendModeControl)stream->readUInt8();
	DestinationFactor = (EGXBlendModeControl)stream->readUInt8();
	Operation = (EGXLogicOp)stream->readUInt8();
}

bool J3DBlendMode::operator==(const J3DBlendMode& other) const {
	return Type == other.Type && SourceFactor == other.SourceFactor && DestinationFactor == other.DestinationFactor &&
		Operation == other.Operation;
}

bool J3DBlendMode::operator!=(const J3DBlendMode& other) const {
	return !operator==(other);
}

/* == J3DFog == */
void J3DFog::Deserialize(bStream::CStream* stream) {
	Type = (EGXFogType)stream->readUInt8();
	Enable = stream->readUInt8();
	Center = stream->readUInt16();
	StartZ = stream->readFloat();
	EndZ = stream->readFloat();
	NearZ = stream->readFloat();
	FarZ = stream->readFloat();

	Color.r = stream->readUInt8() / 255.0f;
	Color.g = stream->readUInt8() / 255.0f;
	Color.b = stream->readUInt8() / 255.0f;
	Color.a = stream->readUInt8() / 255.0f;

	for (int i = 0; i < 10; i++)
		AdjustmentTable[i] = stream->readUInt16();
}

bool J3DFog::operator==(const J3DFog& other) const {
	bool comparison = Type == other.Type && Enable == other.Enable && Center == other.Center &&
		StartZ == other.StartZ && EndZ == other.EndZ && NearZ == other.NearZ && FarZ == other.FarZ && Color == other.Color;

	for (int i = 0; i < 10; i++) {
		bool adjTest = AdjustmentTable[i] == other.AdjustmentTable[i];

		if (!adjTest) {
			comparison = false;
			break;
		}
	}

	return comparison;
}

bool J3DFog::operator!=(const J3DFog& other) const {
	return !operator==(other);
}

/* == J3DColorChannel == */
void J3DColorChannel::Deserialize(bStream::CStream* stream) {
	LightingEnabled = stream->readUInt8();
	MaterialSource = (EGXColorSource)stream->readUInt8();
	LightMask = stream->readUInt8();
	DiffuseFunction = (EGXDiffuseFunction)stream->readUInt8();
	AttenuationFunction = (EGXAttenuationFunction)stream->readUInt8();
	AmbientSource = (EGXColorSource)stream->readUInt8();
}

bool J3DColorChannel::operator==(const J3DColorChannel& other) const {
	return LightingEnabled == other.LightingEnabled && MaterialSource == other.MaterialSource && LightMask == other.LightMask &&
		DiffuseFunction == other.DiffuseFunction && AttenuationFunction == other.AttenuationFunction && AmbientSource == other.AmbientSource;
}

bool J3DColorChannel::operator!=(const J3DColorChannel& other) const {
	return !operator==(other);
}

/* == J3DTexCoordInfo == */
J3DTexCoordInfo::J3DTexCoordInfo() : Type(EGXTexGenType::Matrix2x4), Source(EGXTexGenSrc::TexCoord0), TexMatrix(EGXTexMatrix::Identity) {

}

void J3DTexCoordInfo::Deserialize(bStream::CStream* stream) {
	Type = (EGXTexGenType)stream->readUInt8();
	Source = (EGXTexGenSrc)stream->readUInt8();
	TexMatrix = (EGXTexMatrix)stream->readUInt8();
}

bool J3DTexCoordInfo::operator==(const J3DTexCoordInfo& other) const {
	return Type == other.Type && Source == other.Source && TexMatrix == other.TexMatrix;
}

bool J3DTexCoordInfo::operator!=(const J3DTexCoordInfo& other) const {
	return !operator==(other);
}

/* == J3DTexMatrixInfo == */
J3DTexMatrixInfo::J3DTexMatrixInfo() : Projection(EJ3DTexMatrixProjection::STQ), Type(EGXTexMatrixType::Matrix2x4), Origin(glm::vec3(0.5f, 0.5f, 0.5f)), Matrix(glm::identity<glm::mat4>()) {
	Transform.Scale = glm::vec2(1.0f, 1.0f);
	Transform.Rotation = 0.0f;
	Transform.Translation = glm::vec2(0.0f, 0.0f);
}

void J3DTexMatrixInfo::Deserialize(bStream::CStream* stream) {
	Projection = (EJ3DTexMatrixProjection)stream->readUInt8();
	Type = (EGXTexMatrixType)stream->readUInt8();
	
	stream->skip(2);

	Origin.x = stream->readFloat();
	Origin.y = stream->readFloat();
	Origin.z = stream->readFloat();

	Transform.Deserialize(stream);

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			Matrix[x][y] = stream->readFloat();
		}
	}
}

bool J3DTexMatrixInfo::operator==(const J3DTexMatrixInfo& other) const {
	return Projection == other.Projection && Type == other.Type && Origin == other.Origin && Transform == other.Transform;
}

bool J3DTexMatrixInfo::operator!=(const J3DTexMatrixInfo& other) const {
	return !operator==(other);
}

/* == J3DNBTScaleInfo == */
void J3DNBTScaleInfo::Deserialize(bStream::CStream* stream) {
	Enable = stream->readUInt8();
	
	stream->skip(3);

	Scale.x = stream->readFloat();
	Scale.y = stream->readFloat();
	Scale.z = stream->readFloat();
}

bool J3DNBTScaleInfo::operator==(const J3DNBTScaleInfo& other) const {
	return Enable == other.Enable && Scale == other.Scale;
}

bool J3DNBTScaleInfo::operator!=(const J3DNBTScaleInfo& other) const {
	return !operator==(other);
}

/* == J3DTevOrderInfo == */
J3DTevOrderInfo::J3DTevOrderInfo() : TexCoordId(EGXTexCoordSlot::Null), TexMap(0xFF),
	ChannelId(EGXColorChannelId::Color0A0) {

}

void J3DTevOrderInfo::Deserialize(bStream::CStream* stream) {
	TexCoordId = (EGXTexCoordSlot)stream->readUInt8();
	TexMap = stream->readUInt8();
	ChannelId = (EGXColorChannelId)stream->readUInt8();
}

bool J3DTevOrderInfo::operator==(const J3DTevOrderInfo& other) const {
	return TexCoordId == other.TexCoordId && TexMap == other.TexMap && ChannelId == other.ChannelId;
}

bool J3DTevOrderInfo::operator!=(const J3DTevOrderInfo& other) const {
	return !operator==(other);
}

/* == J3DSwapModeInfo == */
J3DSwapModeInfo::J3DSwapModeInfo() : J3DSwapModeInfo(0, 0) {

}

J3DSwapModeInfo::J3DSwapModeInfo(uint8_t ras, uint8_t tex) : RasIndex(ras), TexIndex(tex) {

}

void J3DSwapModeInfo::Deserialize(bStream::CStream* stream) {
	RasIndex = stream->readUInt8();
	TexIndex = stream->readUInt8();
}

bool J3DSwapModeInfo::operator==(const J3DSwapModeInfo& other) const {
	return RasIndex == other.RasIndex && TexIndex == other.TexIndex;
}

bool J3DSwapModeInfo::operator!=(const J3DSwapModeInfo& other) const {
	return !operator==(other);
}

/* == J3DSwapModeTableInfo == */
J3DSwapModeTableInfo::J3DSwapModeTableInfo() : R(EGXSwapMode::R), G(EGXSwapMode::G), B(EGXSwapMode::B), A(EGXSwapMode::A) {

}

void J3DSwapModeTableInfo::Deserialize(bStream::CStream* stream) {
	R = (EGXSwapMode)stream->readUInt8();
	G = (EGXSwapMode)stream->readUInt8();
	B = (EGXSwapMode)stream->readUInt8();
	A = (EGXSwapMode)stream->readUInt8();
}

bool J3DSwapModeTableInfo::operator==(const J3DSwapModeTableInfo& other) const {
	return R == other.R && G == other.G && B == other.B && A == other.A;
}

bool J3DSwapModeTableInfo::operator!=(const J3DSwapModeTableInfo& other) const {
	return !operator==(other);
}

/* == J3DTevStageInfo == */
J3DTevStageInfo::J3DTevStageInfo() : ColorInput{ EGXCombineColorInput::Zero, EGXCombineColorInput::Zero, EGXCombineColorInput::Zero, EGXCombineColorInput::ColorPrev},
	ColorOperation(EGXTevOp::Add), ColorBias(EGXTevBias::Zero), ColorScale(EGXTevScale::Scale_1), ColorClamp(true), ColorOutputRegister(EGXTevRegister::Prev),
	AlphaInput{ EGXCombineAlphaInput::Zero, EGXCombineAlphaInput::Zero, EGXCombineAlphaInput::Zero, EGXCombineAlphaInput::AlphaPrev},
	AlphaOperation(EGXTevOp::Add), AlphaBias(EGXTevBias::Zero), AlphaScale(EGXTevScale::Scale_1), AlphaClamp(true), AlphaOutputRegister(EGXTevRegister::Prev),
	Unknown0(0), Unknown1(0) {

}

void J3DTevStageInfo::Deserialize(bStream::CStream* stream) {
	Unknown0 = stream->readUInt8();

	// Color
	for (int i = 0; i < 4; i++) {
		ColorInput[i] = (EGXCombineColorInput)stream->readUInt8();
	}

	ColorOperation = (EGXTevOp)stream->readUInt8();
	ColorBias = (EGXTevBias)stream->readUInt8();
	ColorScale = (EGXTevScale)stream->readUInt8();
	ColorClamp = stream->readUInt8();
	ColorOutputRegister = (EGXTevRegister)stream->readUInt8();

	// Alpha
	for (int i = 0; i < 4; i++) {
		AlphaInput[i] = (EGXCombineAlphaInput)stream->readUInt8();
	}

	AlphaOperation = (EGXTevOp)stream->readUInt8();
	AlphaBias = (EGXTevBias)stream->readUInt8();
	AlphaScale = (EGXTevScale)stream->readUInt8();
	AlphaClamp = stream->readUInt8();
	AlphaOutputRegister = (EGXTevRegister)stream->readUInt8();

	Unknown1 = stream->readUInt8();
}

bool J3DTevStageInfo::operator==(const J3DTevStageInfo& other) const {
	bool comparison = ColorOperation == other.ColorOperation && ColorBias == other.ColorBias && ColorScale == other.ColorScale &&
		ColorClamp == other.ColorClamp && ColorOutputRegister == other.ColorOutputRegister && AlphaOperation == other.AlphaOperation &&
		AlphaBias == other.AlphaBias && AlphaScale == other.AlphaScale && AlphaClamp == other.AlphaClamp && AlphaOutputRegister == other.AlphaOutputRegister;

	for (int i = 0; i < 4; i++) {
		bool inputTest = (ColorInput[i] == other.ColorInput[i]) && (AlphaInput[i] == other.AlphaInput[i]);

		if (!inputTest) {
			comparison = false;
			break;
		}
	}

	return comparison;
}

bool J3DTevStageInfo::operator!=(const J3DTevStageInfo& other) const {
	return !operator==(other);
}
