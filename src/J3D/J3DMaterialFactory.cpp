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
	newMaterial->mName = mNameTable.GetName(index);

	// Material init data can be "instanced", meaning that multiple materials use the same init data to load from file.
	// We can know which init data to use by looking up the proper index from the instance table.
	uint16_t instanceIndex = mInstanceTable[index];

	stream->seek(mBlock->InitDataTableOffset + (instanceIndex * sizeof(J3DMaterialInitData)));

	J3DMaterialInitData initData;
	initData.Deserialize(stream);

	newMaterial->mPEMode = (EPixelEngineMode)(initData.PEMode & 0x07);

	// Pixel engine block
	newMaterial->mPEBlock.mZMode = ReadMaterialComponent<J3DZMode>(stream, mBlock->ZModeTableOffset, initData.ZMode);
	newMaterial->mPEBlock.mAlphaCompare = ReadMaterialComponent<J3DAlphaCompare>(stream, mBlock->AlphaCompareTableOffset, initData.AlphaCompare);
	newMaterial->mPEBlock.mBlendMode = ReadMaterialComponent<J3DBlendMode>(stream, mBlock->BlendInfoTableOffset, initData.BlendMode);
	newMaterial->mPEBlock.mFog = ReadMaterialComponent<J3DFog>(stream, mBlock->FogTableOffset, initData.Fog);
	newMaterial->mPEBlock.mZCompLoc = stream->peekUInt8(mBlock->ZCompLocTableOffset + initData.ZCompLoc * sizeof(uint8_t));
	newMaterial->mPEBlock.mDither = stream->peekUInt8(mBlock->DitherTableOffset + initData.Dither * sizeof(uint8_t));

	// Light block
	newMaterial->mLightBlock.mCullMode = (EGXCullMode)stream->peekUInt32(mBlock->CullModeTableOffset + initData.CullMode * sizeof(uint32_t));
	
	for (int i = 0; i < 2; i++) {
		if (initData.MatteColor[i] != UINT16_MAX) {
			newMaterial->mLightBlock.mMatteColor[i].r = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MatteColor[i] * sizeof(glm::vec4));
			newMaterial->mLightBlock.mMatteColor[i].g = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MatteColor[i] * sizeof(glm::vec4) + 1);
			newMaterial->mLightBlock.mMatteColor[i].b = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MatteColor[i] * sizeof(glm::vec4) + 2);
			newMaterial->mLightBlock.mMatteColor[i].a = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MatteColor[i] * sizeof(glm::vec4) + 3);
		}

		if (initData.AmbientColor[i] != UINT16_MAX) {
			newMaterial->mLightBlock.mAmbientColor[i].r = stream->peekUInt8(mBlock->AmbientColorTableOffset + initData.AmbientColor[i] * sizeof(glm::vec4));
			newMaterial->mLightBlock.mAmbientColor[i].g = stream->peekUInt8(mBlock->AmbientColorTableOffset + initData.AmbientColor[i] * sizeof(glm::vec4) + 1);
			newMaterial->mLightBlock.mAmbientColor[i].b = stream->peekUInt8(mBlock->AmbientColorTableOffset + initData.AmbientColor[i] * sizeof(glm::vec4) + 2);
			newMaterial->mLightBlock.mAmbientColor[i].a = stream->peekUInt8(mBlock->AmbientColorTableOffset + initData.AmbientColor[i] * sizeof(glm::vec4) + 3);
		}
	}

	uint8_t chanControlNum = stream->peekUInt8(mBlock->ColorChannelCountTableOffset + initData.ColorChannelCount * sizeof(uint8_t));
	for (int i = 0; i < chanControlNum; i++) {
		newMaterial->mLightBlock.mColorChannels.push_back(ReadMaterialComponent<J3DColorChannel>(stream, mBlock->ColorChannelTableOffset, initData.ColorChannel[i]));
	}

	// Tex gen block
	uint8_t texGenNum = stream->peekUInt8(mBlock->TexGenCountTableOffset + initData.TexGenCount * sizeof(uint8_t));
	for (int i = 0; i < texGenNum; i++) {
		newMaterial->mTexGenBlock.mTexCoordInfo.push_back(ReadMaterialComponent<J3DTexCoordInfo>(stream, mBlock->TexCoordTableOffset, initData.TexCoord[i]));
	}

	for (int i = 0; i < 10; i++) {
		if (initData.TexMatrix[i] == UINT16_MAX)
			continue;

		newMaterial->mTexGenBlock.mTexMatrix.push_back(ReadMaterialComponent<J3DTexMatrixInfo>(stream, mBlock->TexMatrixTableOffset, initData.TexMatrix[i]));
	}

	if (initData.NBTScale != UINT16_MAX) {
		newMaterial->mTexGenBlock.mNBTScale = ReadMaterialComponent<J3DNBTScaleInfo>(stream, mBlock->NBTScaleTableOffset, initData.NBTScale);
	}

	// Tev block
	for (int i = 0; i < 8; i++) {
		if (initData.TextureIndex[i] == UINT16_MAX)
			continue;

		newMaterial->mTevBlock.mTextureIndices.push_back(stream->peekUInt16(mBlock->TextureIndexTableOffset + initData.TextureIndex[i] * sizeof(uint16_t)));
	}

	uint8_t tevStageNum = stream->peekUInt8(mBlock->TevStageCountTableOffset + initData.TEVStageCount * sizeof(uint8_t));
	for (int i = 0; i < tevStageNum; i++) {
		if (initData.TEVOrder[i] != UINT16_MAX)
			newMaterial->mTevBlock.mTevOrders.push_back(ReadMaterialComponent<J3DTevOrderInfo>(stream, mBlock->TevOrderTableOffset, initData.TEVOrder[i]));
	
		if (initData.TEVStage[i] != UINT16_MAX)
			newMaterial->mTevBlock.mTevStages.push_back(ReadMaterialComponent<J3DTevStageInfo>(stream, mBlock->TevStageTableOffset, initData.TEVStage[i]));
	}

	for (int i = 0; i < 16; i++) {
		newMaterial->mTevBlock.mKonstColorSelection[i] = (EGXKonstColorSel)initData.TEVKonstColorSelect[i];
		newMaterial->mTevBlock.mKonstAlphaSelection[i] = (EGXKonstAlphaSel)initData.TEVKonstAlphaSelect[i];
	}

	for (int i = 0; i < 4; i++) {
		if (initData.TEVColor[i] != UINT16_MAX) {
			newMaterial->mTevBlock.mTevColors[i].r = stream->peekUInt8(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::vec4));
			newMaterial->mTevBlock.mTevColors[i].g = stream->peekUInt8(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::vec4) + 1);
			newMaterial->mTevBlock.mTevColors[i].b = stream->peekUInt8(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::vec4) + 2);
			newMaterial->mTevBlock.mTevColors[i].a = stream->peekUInt8(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::vec4) + 3);
		}

		if (initData.TEVKonstColor[i] != UINT16_MAX) {
			newMaterial->mTevBlock.mTevKonstColors[i].r = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t));
			newMaterial->mTevBlock.mTevKonstColors[i].g = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t) + 1);
			newMaterial->mTevBlock.mTevKonstColors[i].b = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t) + 2);
			newMaterial->mTevBlock.mTevKonstColors[i].a = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t) + 3);
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

	MatteColor[0] = stream->readUInt16();
	MatteColor[1] = stream->readUInt16();

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