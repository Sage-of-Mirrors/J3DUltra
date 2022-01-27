#pragma once

#include "J3DNameTable.hpp"

#include <vector>
#include <cstdint>

namespace bStream { class CStream; }
class J3DMaterial;
struct J3DMaterialBlock;

struct J3DMaterialInitData {
	uint8_t PEMode;
	uint8_t CullMode;
	uint8_t ColorChannelCount;
	uint8_t TexGenCount;
	uint8_t TEVStageCount;
	uint8_t ZCompLoc;
	uint8_t ZMode;
	uint8_t Dither;

	uint16_t MatteColor[2];
	uint16_t ColorChannel[4];
	uint16_t AmbientColor[2];
	uint16_t Light[8];
	uint16_t TexCoord[8];
	uint16_t TexCoord2[8];
	uint16_t TexMatrix[10];
	uint16_t PostTexMatrix[20];
	uint16_t TextureIndex[8];
	uint16_t TEVKonstColor[4];

	uint8_t TEVKonstColorSelect[16];
	uint8_t TEVKonstAlphaSelect[16];

	uint16_t TEVOrder[16];
	uint16_t TEVColor[4];
	uint16_t TEVStage[16];
	uint16_t TEVSwapMode[16];
	uint16_t TEVSwapModeTable[16];

	uint16_t Fog;
	uint16_t AlphaCompare;
	uint16_t BlendMode;
	uint16_t NBTScale;

	void Deserialize(bStream::CStream* stream);
};

class J3DMaterialFactory {
	J3DMaterialBlock* mBlock;

	std::vector<uint16_t> mInstanceTable;
	J3DNameTable mNameTable;

	uint8_t ReadMaterialPrimitiveComponent(bStream::CStream* stream, uint32_t offset, uint32_t index);

	template<typename T>
	T ReadMaterialComponent(bStream::CStream* stream, uint32_t offset, uint32_t index) {
		T newComp;
		
		ptrdiff_t currentOffset = stream->tell();
		stream->seek(offset + (index * (sizeof(T) - 12)));

		J3DMaterialComponentBase* tAsBase = static_cast<J3DMaterialComponentBase*>(&newComp);
		if (tAsBase != nullptr)
			tAsBase->Deserialize(stream);

		stream->seek(currentOffset);
		return newComp;
	}

public:
	J3DMaterialFactory(J3DMaterialBlock* srcBlock, bStream::CStream* stream);
	~J3DMaterialFactory() {}

	J3DMaterial* Create(bStream::CStream* stream, uint32_t index);
};
