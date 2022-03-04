#include "J3D/J3DMaterialFactory.hpp"
#include "J3D/J3DBlock.hpp"
#include "J3D/J3DMaterial.hpp"
#include <bstream.h>

J3DMaterialFactory::J3DMaterialFactory(J3DMaterialBlock* srcBlock, bStream::CStream* stream) {
	mBlock = srcBlock;

	stream->seek(mBlock->IndexTableOffset);
	for (int i = 0; i < mBlock->Count; i++)
		mInstanceTable.push_back(stream->readUInt16());

	stream->seek(mBlock->NameTableOffset);
	mNameTable.Deserialize(stream);
}

J3DMaterial* J3DMaterialFactory::Create(bStream::CStream* stream, uint32_t index) {
	J3DMaterial* newMaterial = new J3DMaterial();
	newMaterial->Name = mNameTable.GetName(index);

	// Material init data can be "instanced", meaning that multiple materials use the same init data to load from file.
	// We can know which init data to use by looking up the proper index from the instance table.
	uint16_t instanceIndex = mInstanceTable[index];

	stream->seek(mBlock->InitDataTableOffset + (instanceIndex * sizeof(J3DMaterialInitData)));

	J3DMaterialInitData initData;
	initData.Deserialize(stream);

	newMaterial->PEMode = (EPixelEngineMode)(initData.PEMode & 0x07);

	// Pixel engine block
	newMaterial->PEBlock.mZMode = ReadMaterialComponent<J3DZMode>(stream, mBlock->ZModeTableOffset, initData.ZMode);
	newMaterial->PEBlock.mAlphaCompare = ReadMaterialComponent<J3DAlphaCompare>(stream, mBlock->AlphaCompareTableOffset, initData.AlphaCompare);
	newMaterial->PEBlock.mBlendMode = ReadMaterialComponent<J3DBlendMode>(stream, mBlock->BlendInfoTableOffset, initData.BlendMode);
	newMaterial->PEBlock.mFog = ReadMaterialComponent<J3DFog>(stream, mBlock->FogTableOffset, initData.Fog);
	newMaterial->PEBlock.mZCompLoc = stream->peekUInt8(mBlock->ZCompLocTableOffset + initData.ZCompLoc * sizeof(uint8_t));
	newMaterial->PEBlock.mDither = stream->peekUInt8(mBlock->DitherTableOffset + initData.Dither * sizeof(uint8_t));

	// Light block
	newMaterial->LightBlock.mCullMode = (EGXCullMode)stream->peekUInt32(mBlock->CullModeTableOffset + initData.CullMode * sizeof(uint32_t));
	
	for (int i = 0; i < 2; i++) {
		if (initData.MaterialColor[i] != UINT16_MAX) {
			newMaterial->LightBlock.mMatteColor[i].r = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MaterialColor[i] * sizeof(glm::vec4));
			newMaterial->LightBlock.mMatteColor[i].g = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MaterialColor[i] * sizeof(glm::vec4) + 1);
			newMaterial->LightBlock.mMatteColor[i].b = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MaterialColor[i] * sizeof(glm::vec4) + 2);
			newMaterial->LightBlock.mMatteColor[i].a = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MaterialColor[i] * sizeof(glm::vec4) + 3);
		}

		if (initData.AmbientColor[i] != UINT16_MAX) {
			newMaterial->LightBlock.mAmbientColor[i].r = stream->peekUInt8(mBlock->AmbientColorTableOffset + initData.AmbientColor[i] * sizeof(glm::vec4));
			newMaterial->LightBlock.mAmbientColor[i].g = stream->peekUInt8(mBlock->AmbientColorTableOffset + initData.AmbientColor[i] * sizeof(glm::vec4) + 1);
			newMaterial->LightBlock.mAmbientColor[i].b = stream->peekUInt8(mBlock->AmbientColorTableOffset + initData.AmbientColor[i] * sizeof(glm::vec4) + 2);
			newMaterial->LightBlock.mAmbientColor[i].a = stream->peekUInt8(mBlock->AmbientColorTableOffset + initData.AmbientColor[i] * sizeof(glm::vec4) + 3);
		}
	}

	uint8_t chanControlNum = stream->peekUInt8(mBlock->ColorChannelCountTableOffset + initData.ColorChannelCount * sizeof(uint8_t));
	for (int i = 0; i < chanControlNum; i++) {
		newMaterial->LightBlock.mColorChannels.push_back(ReadMaterialComponent<J3DColorChannel>(stream, mBlock->ColorChannelTableOffset, initData.ColorChannel[i]));
	}

	// Tex gen block
	uint8_t texGenNum = stream->peekUInt8(mBlock->TexGenCountTableOffset + initData.TexGenCount * sizeof(uint8_t));
	for (int i = 0; i < texGenNum; i++) {
		newMaterial->TexGenBlock.mTexCoordInfo.push_back(ReadMaterialComponent<J3DTexCoordInfo>(stream, mBlock->TexCoordTableOffset, initData.TexCoord[i]));

		if (initData.TexCoord2[i] != UINT16_MAX)
			newMaterial->TexGenBlock.mTexCoordInfo.push_back(ReadMaterialComponent<J3DTexCoordInfo>(stream, mBlock->TexCoord2TableOffset, initData.TexCoord2[i]));
	}

	for (int i = 0; i < 10; i++) {
		if (initData.TexMatrix[i] == UINT16_MAX)
			continue;

		newMaterial->TexGenBlock.mTexMatrix.push_back(ReadMaterialComponent<J3DTexMatrixInfo>(stream, mBlock->TexMatrixTableOffset, initData.TexMatrix[i]));
	}

	if (initData.NBTScale != UINT16_MAX) {
		newMaterial->TexGenBlock.mNBTScale = ReadMaterialComponent<J3DNBTScaleInfo>(stream, mBlock->NBTScaleTableOffset, initData.NBTScale);
	}

	// Tev block
	for (int i = 0; i < 8; i++) {
		if (initData.TextureIndex[i] == UINT16_MAX)
			continue;

		newMaterial->TevBlock.mTextureIndices.push_back(stream->peekUInt16(mBlock->TextureIndexTableOffset + initData.TextureIndex[i] * sizeof(uint16_t)));
	}

	uint8_t tevStageNum = stream->peekUInt8(mBlock->TevStageCountTableOffset + initData.TEVStageCount * sizeof(uint8_t));
	for (int i = 0; i < tevStageNum; i++) {
		if (initData.TEVOrder[i] != UINT16_MAX)
			newMaterial->TevBlock.mTevOrders.push_back(ReadMaterialComponent<J3DTevOrderInfo>(stream, mBlock->TevOrderTableOffset, initData.TEVOrder[i]));

		uint32_t currentOffset = stream->tell();
		if (initData.TEVSwapMode[i] != UINT16_MAX) {
			uint8_t tex, ras;
			stream->seek(mBlock->TevSwapModeTableOffset + (initData.TEVSwapMode[i] * 4));
			tex = stream->readUInt8();
			ras = stream->readUInt8();

			stream->seek(mBlock->TevSwapTableOffset + initData.TEVSwapModeTable[tex] * 4);
			newMaterial->TevBlock.mTevOrders[i].mTexSwapTable[0] = stream->readUInt8();
			newMaterial->TevBlock.mTevOrders[i].mTexSwapTable[1] = stream->readUInt8();
			newMaterial->TevBlock.mTevOrders[i].mTexSwapTable[2] = stream->readUInt8();
			newMaterial->TevBlock.mTevOrders[i].mTexSwapTable[3] = stream->readUInt8();

			stream->seek(mBlock->TevSwapTableOffset + initData.TEVSwapModeTable[ras] * 4);
			newMaterial->TevBlock.mTevOrders[i].mRasSwapTable[0] = stream->readUInt8();
			newMaterial->TevBlock.mTevOrders[i].mRasSwapTable[1] = stream->readUInt8();
			newMaterial->TevBlock.mTevOrders[i].mRasSwapTable[2] = stream->readUInt8();
			newMaterial->TevBlock.mTevOrders[i].mRasSwapTable[3] = stream->readUInt8();
		}
		stream->seek(currentOffset);

		if (initData.TEVStage[i] != UINT16_MAX)
			newMaterial->TevBlock.mTevStages.push_back(ReadMaterialComponent<J3DTevStageInfo>(stream, mBlock->TevStageTableOffset, initData.TEVStage[i]));
	}

	for (int i = 0; i < 16; i++) {
		newMaterial->TevBlock.mKonstColorSelection[i] = (EGXKonstColorSel)initData.TEVKonstColorSelect[i];
		newMaterial->TevBlock.mKonstAlphaSelection[i] = (EGXKonstAlphaSel)initData.TEVKonstAlphaSelect[i];
	}

	for (int i = 0; i < 4; i++) {
		if (initData.TEVColor[i] != UINT16_MAX) {
			newMaterial->TevBlock.mTevColors[i].r = stream->peekUInt8(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::vec4));
			newMaterial->TevBlock.mTevColors[i].g = stream->peekUInt8(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::vec4) + 1);
			newMaterial->TevBlock.mTevColors[i].b = stream->peekUInt8(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::vec4) + 2);
			newMaterial->TevBlock.mTevColors[i].a = stream->peekUInt8(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::vec4) + 3);
		}

		if (initData.TEVKonstColor[i] != UINT16_MAX) {
			newMaterial->TevBlock.mTevKonstColors[i].r = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t));
			newMaterial->TevBlock.mTevKonstColors[i].g = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t) + 1);
			newMaterial->TevBlock.mTevKonstColors[i].b = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t) + 2);
			newMaterial->TevBlock.mTevKonstColors[i].a = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t) + 3);
		}
	}

	return newMaterial;
}

void J3DMaterialInitData::Deserialize(bStream::CStream* stream) {
	PEMode = stream->readUInt8();
	CullMode = stream->readUInt8();
	ColorChannelCount = stream->readUInt8();
	TexGenCount = stream->readUInt8();
	TEVStageCount = stream->readUInt8();
	ZCompLoc = stream->readUInt8();
	ZMode = stream->readUInt8();
	Dither = stream->readUInt8();

	MaterialColor[0] = stream->readUInt16();
	MaterialColor[1] = stream->readUInt16();

	for (int i = 0; i < 4; i++)
		ColorChannel[i] = stream->readUInt16();

	AmbientColor[0] = stream->readUInt16();
	AmbientColor[1] = stream->readUInt16();

	for (int i = 0; i < 8; i++)
		Light[i] = stream->readUInt16();

	for (int i = 0; i < 8; i++)
		TexCoord[i] = stream->readUInt16();
	for (int i = 0; i < 8; i++)
		TexCoord2[i] = stream->readUInt16();

	for (int i = 0; i < 10; i++)
		TexMatrix[i] = stream->readUInt16();
	for (int i = 0; i < 20; i++)
		PostTexMatrix[i] = stream->readUInt16();

	for (int i = 0; i < 8; i++)
		TextureIndex[i] = stream->readUInt16();

	for (int i = 0; i < 4; i++)
		TEVKonstColor[i] = stream->readUInt16();

	for (int i = 0; i < 16; i++)
		TEVKonstColorSelect[i] = stream->readUInt8();
	for (int i = 0; i < 16; i++)
		TEVKonstAlphaSelect[i] = stream->readUInt8();

	for (int i = 0; i < 16; i++)
		TEVOrder[i] = stream->readUInt16();
	for (int i = 0; i < 4; i++)
		TEVColor[i] = stream->readUInt16();
	for (int i = 0; i < 16; i++)
		TEVStage[i] = stream->readUInt16();
	for (int i = 0; i < 16; i++)
		TEVSwapMode[i] = stream->readUInt16();
	for (int i = 0; i < 16; i++)
		TEVSwapModeTable[i] = stream->readUInt16();

	Fog = stream->readUInt16();
	AlphaCompare = stream->readUInt16();
	BlendMode = stream->readUInt16();
	NBTScale = stream->readUInt16();
}