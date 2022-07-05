#include "J3D/J3DMaterialData.hpp"
#include <bstream.h>

void J3DZMode::Deserialize(bStream::CStream* stream) {
	Enable = stream->readUInt8();
	Function = (EGXCompareType)stream->readUInt8();
	UpdateEnable = stream->readUInt8();
}

void J3DAlphaCompare::Deserialize(bStream::CStream* stream) {
	CompareFunc0 = (EGXCompareType)stream->readUInt8();
	Reference0 = stream->readUInt8();

	Operation = (EGXAlphaOp)stream->readUInt8();

	CompareFunc1 = (EGXCompareType)stream->readUInt8();
	Reference1 = stream->readUInt8();
}

void J3DBlendMode::Deserialize(bStream::CStream* stream) {
	Type = (EGXBlendMode)stream->readUInt8();
	SourceFactor = (EGXBlendModeControl)stream->readUInt8();
	DestinationFactor = (EGXBlendModeControl)stream->readUInt8();
	Operation = (EGXLogicOp)stream->readUInt8();
}

void J3DFog::Deserialize(bStream::CStream* stream) {
	Type = (EGXFogType)stream->readUInt8();
	Enable = stream->readUInt8();
	Center = stream->readUInt16();
	StartZ = stream->readFloat();
	EndZ = stream->readFloat();
	NearZ = stream->readFloat();
	FarZ = stream->readFloat();

	Color.r = stream->readUInt8();
	Color.g = stream->readUInt8();
	Color.b = stream->readUInt8();
	Color.a = stream->readUInt8();

	for (int i = 0; i < 10; i++)
		AdjustmentTable[i] = stream->readUInt16();
}

void J3DColorChannel::Deserialize(bStream::CStream* stream) {
	LightingEnabled = stream->readUInt8();
	MaterialSource = (EGXColorSource)stream->readUInt8();
	LightMask = stream->readUInt8();
	DiffuseFunction = (EGXDiffuseFunction)stream->readUInt8();
	AttenuationFunction = (EGXAttenuationFunction)stream->readUInt8();
	AmbientSource = (EGXColorSource)stream->readUInt8();
}

void J3DTexCoordInfo::Deserialize(bStream::CStream* stream) {
	Type = (EGXTexGenType)stream->readUInt8();
	Source = (EGXTexGenSrc)stream->readUInt8();
	TexMatrix = (EGXTexMatrix)stream->readUInt8();
}

void J3DTexMatrixInfo::Deserialize(bStream::CStream* stream) {
	Projection = (EJ3DTexMatrixProjection)stream->readUInt8();
	Type = stream->readUInt8();
	
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

void J3DNBTScaleInfo::Deserialize(bStream::CStream* stream) {
	Enable = stream->readUInt8();
	
	stream->skip(3);

	Scale.x = stream->readFloat();
	Scale.y = stream->readFloat();
	Scale.z = stream->readFloat();
}

void J3DTevOrderInfo::Deserialize(bStream::CStream* stream) {
	TexCoordId = (EGXTexCoordSlot)stream->readUInt8();
	TexMap = stream->readUInt8();
	ChannelId = (EGXColorChannelId)stream->readUInt8();
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
