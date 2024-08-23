#include "J3D/Material/J3DMaterialData.hpp"
#include "J3D/Util/J3DTransform.hpp"

#include <bstream.h>
#include <glm/matrix.hpp>

/* == J3DZMode == */
void J3DZMode::Serialize(bStream::CStream* stream) {
	stream->writeUInt8(Enable);
	stream->writeUInt8((uint8_t)Function);
	stream->writeUInt8(UpdateEnable);
	stream->writeUInt8(UINT8_MAX);
}

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
void J3DAlphaCompare::Serialize(bStream::CStream* stream) {
	stream->writeUInt8((uint8_t)CompareFunc0);
	stream->writeUInt8(Reference0);

	stream->writeUInt8((uint8_t)Operation);

	stream->writeUInt8((uint8_t)CompareFunc1);
	stream->writeUInt8(Reference1);

	stream->writeUInt8(UINT8_MAX);
	stream->writeUInt16(UINT16_MAX);
}

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
void J3DBlendMode::Serialize(bStream::CStream* stream) {
	stream->writeUInt8((uint8_t)Type);
	stream->writeUInt8((uint8_t)SourceFactor);
	stream->writeUInt8((uint8_t)DestinationFactor);
	stream->writeUInt8((uint8_t)Operation);
}

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
void J3DFog::Serialize(bStream::CStream* stream) {
	stream->writeUInt8((uint8_t)Type);
	stream->writeUInt8(Enable);
	stream->writeUInt16((uint16_t)Center);

	stream->writeFloat(StartZ);
	stream->writeFloat(EndZ);
	stream->writeFloat(NearZ);
	stream->writeFloat(FarZ);

	stream->writeUInt8(Color.r * 255);
	stream->writeUInt8(Color.g * 255);
	stream->writeUInt8(Color.b * 255);
	stream->writeUInt8(Color.a * 255);

	for (int i = 0; i < 10; i++) {
		stream->writeUInt16(AdjustmentTable[i]);
	}
}

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

	for (int i = 0; i < 10; i++) {
		AdjustmentTable[i] = stream->readUInt16();
	}
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
J3DColorChannel::J3DColorChannel() : LightingEnabled(false), MaterialSource(EGXColorSource::Register), LightMask(0),
	DiffuseFunction(EGXDiffuseFunction::None), AttenuationFunction(EGXAttenuationFunction::None), AmbientSource(EGXColorSource::Register) {

}

void J3DColorChannel::Serialize(bStream::CStream* stream) {
	stream->writeUInt8(LightingEnabled);
	stream->writeUInt8((uint8_t)MaterialSource);
	stream->writeUInt8(LightMask);
	stream->writeUInt8((uint8_t)DiffuseFunction);
	stream->writeUInt8((uint8_t)AttenuationFunction);
	stream->writeUInt8((uint8_t)AmbientSource);
	stream->writeUInt16(UINT16_MAX);
}

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

void J3DTexCoordInfo::Serialize(bStream::CStream* stream) {
	stream->writeUInt8((uint8_t)Type);
	stream->writeUInt8((uint8_t)Source);
	stream->writeUInt8((uint8_t)TexMatrix);
	stream->writeUInt8(UINT8_MAX);
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
J3DTexMatrixInfo::J3DTexMatrixInfo() : Type(EGXTexMatrixType::Matrix2x4), CalcType(EJ3DMatrixCalcType::SOFTIMAGE), TexEffect(EJ3DTexEffect::NONE), Origin(glm::vec3(0.5f, 0.5f, 0.5f)), ProjectionMatrix(glm::identity<glm::mat4>()) {
	Transform.Scale = glm::vec2(1.0f, 1.0f);
	Transform.Rotation = 0.0f;
	Transform.Translation = glm::vec2(0.0f, 0.0f);
}

void J3DTexMatrixInfo::Serialize(bStream::CStream* stream) {
	stream->writeUInt8((uint8_t)Type);

	uint8_t attributes = ((int)(CalcType) << 7) | ((int)(TexEffect));
	stream->writeUInt8(attributes);
	stream->writeUInt16(UINT16_MAX);

	stream->writeFloat(Origin.x);
	stream->writeFloat(Origin.y);
	stream->writeFloat(Origin.z);

	Transform.Serialize(stream);

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			stream->writeFloat(ProjectionMatrix[y][x]);
		}
	}
}

void J3DTexMatrixInfo::Deserialize(bStream::CStream* stream) {
	Type = (EGXTexMatrixType)stream->readUInt8();
	uint8_t attributes = stream->readUInt8();

	CalcType = (EJ3DMatrixCalcType)(attributes & 0x80);
	TexEffect = (EJ3DTexEffect)(attributes & 0x7F);
	
	stream->skip(2);

	Origin.x = stream->readFloat();
	Origin.y = stream->readFloat();
	Origin.z = stream->readFloat();

	Transform.Deserialize(stream);

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			ProjectionMatrix[y][x] = stream->readFloat();
		}
	}
}

void J3DTexMatrixInfo::CalculateMatrix(const glm::mat4& modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) {
	glm::mat4 inputMtx = CalculateInputMatrix(viewMtx * modelMtx, modelMtx);
	glm::mat4 srtMtx = CalculateSRTMatrix();

	CalculateEffectMatrix(inputMtx, srtMtx, modelMtx, projMtx);
}

glm::mat4 J3DTexMatrixInfo::CalculateInputMatrix(const glm::mat4& modelViewMtx, const glm::mat4& modelMtx) {
	switch (TexEffect) {
		case EJ3DTexEffect::ENVMAP_BASIC:
		case EJ3DTexEffect::ENVMAP_OLD:
		case EJ3DTexEffect::ENVMAP:
			return glm::mat4(modelViewMtx[0], modelViewMtx[1], modelViewMtx[2], { 0.0f, 0.0f, 0.0f, 1.0f });
		case EJ3DTexEffect::PROJMAP_BASIC:
		case EJ3DTexEffect::PROJMAP:
			return modelMtx;
		case EJ3DTexEffect::VIEWPROJMAP_BASIC:
		case EJ3DTexEffect::VIEWPROJMAP:
			return modelViewMtx;
		case EJ3DTexEffect::EFFECT_5:
		case EJ3DTexEffect::ENVMAP_OLD_EFFECTMTX:
		case EJ3DTexEffect::ENVMAP_EFFECTMTX:
			return glm::mat4(modelMtx[0], modelMtx[1], modelMtx[2], { 0.0f, 0.0f, 0.0f, 1.0f });
		default:
			return glm::identity<glm::mat4>();
	}
}

glm::mat4 J3DTexMatrixInfo::CalculateSRTMatrix() {
	glm::mat4 srtMtx = glm::identity<glm::mat4>();

	if (CalcType == EJ3DMatrixCalcType::SOFTIMAGE) {
		srtMtx[0][0] = Transform.Scale.x * glm::cos(Transform.Rotation);
		srtMtx[0][1] = Transform.Scale.x * -glm::sin(Transform.Rotation);

		srtMtx[0][3] = -glm::cos(Transform.Rotation) * Origin.x +
			glm::sin(Transform.Rotation) * Origin.y +
			Origin.x + Transform.Translation.x;

		srtMtx[1][0] = Transform.Scale.y * glm::sin(Transform.Rotation);
		srtMtx[1][1] = Transform.Scale.y * glm::cos(Transform.Rotation);

		srtMtx[1][3] = glm::sin(Transform.Rotation) * Origin.x -
			glm::cos(Transform.Rotation) * Origin.y +
			Origin.y + Transform.Translation.y;
	}
	else {
		srtMtx[0][0] = Transform.Scale.x * glm::cos(Transform.Rotation);
		srtMtx[0][1] = Transform.Scale.y * glm::sin(Transform.Rotation);

		srtMtx[0][2] = (
			(Transform.Translation.x - 0.5f) * glm::cos(Transform.Rotation) -
			(Transform.Translation.y - 0.5f + Transform.Scale.y) * glm::sin(Transform.Rotation) + 0.5f
		);

		srtMtx[1][0] = Transform.Scale.x * -glm::sin(Transform.Rotation);
		srtMtx[1][1] = Transform.Scale.y * glm::cos(Transform.Rotation);

		srtMtx[1][2] = (
			-(Transform.Translation.x - 0.5f) * glm::sin(Transform.Rotation) -
			(Transform.Translation.y - 0.5f + Transform.Scale.y) * glm::cos(Transform.Rotation) + 0.5f
		);
	}

	srtMtx[3][3] = 1.0f;

	return srtMtx;
}

glm::mat4 envMtxOld = {
	{ 0.5f, 0.0f, 0.0f, 0.0f },
	{ 0.0f,-0.5f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 0.5f, 0.5f, 0.0f, 1.0f }
};

glm::mat4 envMtx = {
	{ 0.5f, 0.0f, 0.0f, 0.0f },
	{ 0.0f,-0.5f, 0.0f, 0.0f },
	{ 0.5f, 0.5f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f }
};

glm::mat4 J3DTexMatrixInfo::CalculateViewProjMatrix(const glm::mat4& projMtx) {
	glm::mat4 pMtx = glm::identity<glm::mat4>();

	pMtx[0][0] = projMtx[0][0] * 0.5f;
	pMtx[1][0] = 0.0f;
	pMtx[2][0] = projMtx[2][0] * 0.5f + projMtx[2][3] * 0.5f;
	pMtx[3][0] = projMtx[3][0] * 0.5f + projMtx[3][3] * 0.5f;

	pMtx[0][1] = 0.0f;
	pMtx[1][1] = projMtx[1][1] * -0.5f;
	pMtx[2][1] = projMtx[2][1] * -0.5f + projMtx[2][3] * 0.5f;
	pMtx[3][1] = projMtx[3][1] * -0.5f + projMtx[3][3] * 0.5f;

	pMtx[0][2] = 0.0f;
	pMtx[1][2] = 0.0f;
	pMtx[2][2] = projMtx[2][3];
	pMtx[3][2] = projMtx[3][3];

	return glm::transpose(pMtx);
}

void J3DTexMatrixInfo::CalculateEffectMatrix(const glm::mat4 inputMtx, const glm::mat4& srtMtx, const glm::mat4& modelMtx, const glm::mat4& projMtx) {
	switch (TexEffect) {
		case EJ3DTexEffect::ENVMAP_BASIC:
		{
			CalculatedMatrix = srtMtx * inputMtx;
			break;
		}
		case EJ3DTexEffect::PROJMAP_BASIC:
		case EJ3DTexEffect::VIEWPROJMAP_BASIC:
		case EJ3DTexEffect::EFFECT_5:
		{
			CalculatedMatrix = (srtMtx * ProjectionMatrix) * inputMtx;
			break;
		}
		case EJ3DTexEffect::EFFECT_4:
		{
			CalculatedMatrix = srtMtx * ProjectionMatrix;
			break;
		}
		case EJ3DTexEffect::ENVMAP_OLD:
		{
			CalculatedMatrix = (srtMtx * envMtxOld) * inputMtx;
			break;
		}
		case EJ3DTexEffect::ENVMAP:
		{
			glm::mat4 texMtx = srtMtx;
			texMtx[2][0] = texMtx[3][0];
			texMtx[2][1] = texMtx[3][1];
			texMtx[2][2] = 1.0f;

			texMtx[3][0] = 0.0f;
			texMtx[3][1] = 0.0f;
			texMtx[3][2] = 0.0f;

			CalculatedMatrix = (texMtx * envMtx) * inputMtx;

			break;
		}
		case EJ3DTexEffect::PROJMAP:
		case EJ3DTexEffect::VIEWPROJMAP:
		case EJ3DTexEffect::ENVMAP_EFFECTMTX:
		{
			glm::mat4 outMtx = glm::identity<glm::mat4>();

			glm::mat4 texMtx = srtMtx;
			texMtx[2][0] = texMtx[3][0];
			texMtx[2][1] = texMtx[3][1];
			texMtx[2][2] = 1.0f;

			texMtx[3][0] = 0.0f;
			texMtx[3][1] = 0.0f;
			texMtx[3][2] = 0.0f;

			if (TexEffect == EJ3DTexEffect::VIEWPROJMAP) {
				outMtx = texMtx * CalculateViewProjMatrix(projMtx);
			}
			else if (TexEffect == EJ3DTexEffect::PROJMAP) {
				outMtx = (texMtx * envMtx) * ProjectionMatrix;
			}
			else {
				outMtx = (texMtx * envMtx) * ProjectionMatrix;
			}

			CalculatedMatrix = outMtx * inputMtx;

			break;
		}
		case EJ3DTexEffect::ENVMAP_OLD_EFFECTMTX:
		{
			CalculatedMatrix = (srtMtx * envMtxOld) * ProjectionMatrix * inputMtx;
			break;
		}
		case EJ3DTexEffect::NONE:
		{
			CalculatedMatrix = srtMtx;
			break;
		}
	}
}

bool J3DTexMatrixInfo::operator==(const J3DTexMatrixInfo& other) const {
	return Type == other.Type && CalcType == other.CalcType && TexEffect == other.TexEffect && Origin == other.Origin && Transform == other.Transform;
}

bool J3DTexMatrixInfo::operator!=(const J3DTexMatrixInfo& other) const {
	return !operator==(other);
}

/* == J3DNBTScaleInfo == */
void J3DNBTScaleInfo::Serialize(bStream::CStream* stream) {
	stream->writeUInt8(Enable);
	stream->writeUInt8(UINT8_MAX);
	stream->writeUInt16(UINT16_MAX);

	stream->writeFloat(Scale.x);
	stream->writeFloat(Scale.y);
	stream->writeFloat(Scale.z);
}

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
J3DTevOrderInfo::J3DTevOrderInfo() : TexCoordId(EGXTexCoordSlot::Null), TexMapId(EGXTexMapSlot::Null),
	ChannelId(EGXColorChannelId::Color0A0) {

}

void J3DTevOrderInfo::Serialize(bStream::CStream* stream) {
	stream->writeUInt8((uint8_t)TexCoordId);
	stream->writeUInt8((uint8_t)TexMapId);
	stream->writeUInt8((uint8_t)ChannelId);
	stream->writeUInt8(UINT8_MAX);
}

void J3DTevOrderInfo::Deserialize(bStream::CStream* stream) {
	TexCoordId = (EGXTexCoordSlot)stream->readUInt8();
	TexMapId = (EGXTexMapSlot)stream->readUInt8();
	ChannelId = (EGXColorChannelId)stream->readUInt8();
}

bool J3DTevOrderInfo::operator==(const J3DTevOrderInfo& other) const {
	return TexCoordId == other.TexCoordId && TexMapId == other.TexMapId && ChannelId == other.ChannelId;
}

bool J3DTevOrderInfo::operator!=(const J3DTevOrderInfo& other) const {
	return !operator==(other);
}

/* == J3DSwapModeInfo == */
J3DSwapModeInfo::J3DSwapModeInfo() : J3DSwapModeInfo(0, 0) {

}

J3DSwapModeInfo::J3DSwapModeInfo(uint8_t ras, uint8_t tex) : RasIndex(ras), TexIndex(tex) {

}

void J3DSwapModeInfo::Serialize(bStream::CStream* stream) {
	stream->writeUInt8(RasIndex);
	stream->writeUInt8(TexIndex);
	stream->writeUInt8(UINT8_MAX);
	stream->writeUInt8(UINT8_MAX);
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

void J3DSwapModeTableInfo::Serialize(bStream::CStream* stream) {
	stream->writeUInt8((uint8_t)R);
	stream->writeUInt8((uint8_t)G);
	stream->writeUInt8((uint8_t)B);
	stream->writeUInt8((uint8_t)A);
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

void J3DTevStageInfo::Serialize(bStream::CStream* stream) {
	stream->writeUInt8(UINT8_MAX);

	for (int i = 0; i < 4; i++) {
		stream->writeUInt8((uint8_t)ColorInput[i]);
	}

	stream->writeUInt8((uint8_t)ColorOperation);
	stream->writeUInt8((uint8_t)ColorBias);
	stream->writeUInt8((uint8_t)ColorScale);
	stream->writeUInt8((uint8_t)ColorClamp);
	stream->writeUInt8((uint8_t)ColorOutputRegister);

	for (int i = 0; i < 4; i++) {
		stream->writeUInt8((uint8_t)AlphaInput[i]);
	}

	stream->writeUInt8((uint8_t)AlphaOperation);
	stream->writeUInt8((uint8_t)AlphaBias);
	stream->writeUInt8((uint8_t)AlphaScale);
	stream->writeUInt8((uint8_t)AlphaClamp);
	stream->writeUInt8((uint8_t)AlphaOutputRegister);

	stream->writeUInt8(UINT8_MAX);
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

/* == J3DIndirectTexMatrixInfo == */
J3DIndirectTexMatrixInfo::J3DIndirectTexMatrixInfo() : TexMatrix(glm::identity<glm::mat2x3>()), Exponent(0) {

}

void J3DIndirectTexMatrixInfo::Serialize(bStream::CStream* stream) {
	stream->writeFloat(TexMatrix[0].x);
	stream->writeFloat(TexMatrix[0].y);
	stream->writeFloat(TexMatrix[0].z);

	stream->writeFloat(TexMatrix[1].x);
	stream->writeFloat(TexMatrix[1].y);
	stream->writeFloat(TexMatrix[1].z);

	stream->writeInt8(Exponent);
	stream->writeUInt8(0xFF);
	stream->writeUInt8(0xFF);
	stream->writeUInt8(0xFF);
}

void J3DIndirectTexMatrixInfo::Deserialize(bStream::CStream* stream) {
	TexMatrix = glm::mat2x3();
	TexMatrix[0].x = stream->readFloat();
	TexMatrix[0].y = stream->readFloat();
	TexMatrix[0].z = stream->readFloat();
	TexMatrix[1].x = stream->readFloat();
	TexMatrix[1].y = stream->readFloat();
	TexMatrix[1].z = stream->readFloat();

	Exponent = stream->readInt8();

	stream->skip(3);

	TexMatrix *= std::pow(2, Exponent);
}

bool J3DIndirectTexMatrixInfo::operator==(const J3DIndirectTexMatrixInfo& other) const {
	return TexMatrix == other.TexMatrix && Exponent == other.Exponent;
}

bool J3DIndirectTexMatrixInfo::operator!=(const J3DIndirectTexMatrixInfo& other) const {
	return !operator==(other);
}

/* == J3DIndirectTexScaleInfo == */
J3DIndirectTexScaleInfo::J3DIndirectTexScaleInfo() : ScaleS(EGXIndirectTexScale::IndDivide_1), ScaleT(EGXIndirectTexScale::IndDivide_1) {

}

void J3DIndirectTexScaleInfo::Serialize(bStream::CStream* stream) {
	stream->writeUInt8((uint8_t)ScaleS);
	stream->writeUInt8((uint8_t)ScaleT);

	stream->writeUInt16(UINT16_MAX);
}

void J3DIndirectTexScaleInfo::Deserialize(bStream::CStream* stream) {
	ScaleS = (EGXIndirectTexScale)stream->readUInt8();
	ScaleT = (EGXIndirectTexScale)stream->readUInt8();

	stream->skip(2);
}

bool J3DIndirectTexScaleInfo::operator==(const J3DIndirectTexScaleInfo& other) const {
	return ScaleS == other.ScaleS && ScaleT == other.ScaleT;
}

bool J3DIndirectTexScaleInfo::operator!=(const J3DIndirectTexScaleInfo& other) const {
	return !operator==(other);
}

/* == J3DIndirectTexOrderInfo == */
J3DIndirectTexOrderInfo::J3DIndirectTexOrderInfo() : TexCoordId(EGXTexCoordSlot::Null), TexMapId(EGXTexMapSlot::Null) {

}

void J3DIndirectTexOrderInfo::Serialize(bStream::CStream* stream) {
	stream->writeUInt8((uint8_t)TexCoordId);
	stream->writeUInt8((uint8_t)TexMapId);

	stream->writeUInt16(UINT16_MAX);
}

void J3DIndirectTexOrderInfo::Deserialize(bStream::CStream* stream) {
	TexCoordId = (EGXTexCoordSlot)stream->readUInt8();
	TexMapId = (EGXTexMapSlot)stream->readUInt8();

	stream->skip(2);
}

bool J3DIndirectTexOrderInfo::operator==(const J3DIndirectTexOrderInfo& other) const {
	return TexCoordId == other.TexCoordId && TexMapId == other.TexMapId;
}

bool J3DIndirectTexOrderInfo::operator!=(const J3DIndirectTexOrderInfo& other) const {
	return !operator==(other);
}

/* == J3DIndirectTevStageInfo == */
J3DIndirectTevStageInfo::J3DIndirectTevStageInfo() : TevStageId(EGXTevStageId::TevStage_0), TexFormat(EGXIndirectTexFormat::IndFormat_8),
	TexBias(EGXIndirectTexBias::IndBias_S), TexMtxId(EGXIndirectTexMatrixId::IndTexMtx_0), TexWrapS(EGXIndirectWrapMode::IndWrapMode_Off),
	TexWrapT(EGXIndirectWrapMode::IndWrapMode_Off), AddPrev(false), UtcLod(false), AlphaSel(EGXIndirectAlphaSel::IndAlphaSel_Off) {

}

void J3DIndirectTevStageInfo::Serialize(bStream::CStream* stream) {
	stream->writeUInt8((uint8_t)TevStageId);
	stream->writeUInt8((uint8_t)TexFormat);
	stream->writeUInt8((uint8_t)TexBias);
	stream->writeUInt8((uint8_t)TexMtxId);
	stream->writeUInt8((uint8_t)TexWrapS);
	stream->writeUInt8((uint8_t)TexWrapT);
	stream->writeUInt8((uint8_t)AddPrev);
	stream->writeUInt8((uint8_t)UtcLod);
	stream->writeUInt8((uint8_t)AlphaSel);

	stream->writeUInt8(UINT8_MAX);
	stream->writeUInt8(UINT8_MAX);
	stream->writeUInt8(UINT8_MAX);
}

void J3DIndirectTevStageInfo::Deserialize(bStream::CStream* stream) {
	TevStageId = (EGXTevStageId)stream->readUInt8();
	TexFormat = (EGXIndirectTexFormat)stream->readUInt8();
	TexBias = (EGXIndirectTexBias)stream->readUInt8();
	TexMtxId = (EGXIndirectTexMatrixId)stream->readUInt8();
	TexWrapS = (EGXIndirectWrapMode)stream->readUInt8();
	TexWrapT = (EGXIndirectWrapMode)stream->readUInt8();
	AddPrev = stream->readUInt8();
	UtcLod = stream->readUInt8();
	AlphaSel = (EGXIndirectAlphaSel)stream->readUInt8();

	stream->skip(3);
}

bool J3DIndirectTevStageInfo::operator==(const J3DIndirectTevStageInfo& other) const {
	return TevStageId == other.TevStageId && TexFormat == other.TexFormat && TexBias == other.TexBias && TexMtxId == other.TexMtxId
		&& TexWrapS == other.TexWrapS && TexWrapT == other.TexWrapT && AddPrev == other.AddPrev && UtcLod == other.UtcLod
		&& AlphaSel == other.AlphaSel;
}

bool J3DIndirectTevStageInfo::operator!=(const J3DIndirectTevStageInfo& other) const {
	return !operator==(other);
}
