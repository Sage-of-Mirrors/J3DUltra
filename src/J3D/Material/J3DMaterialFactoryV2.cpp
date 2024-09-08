#include "J3D/Material/J3DMaterialFactoryV2.hpp"
#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Data/J3DBlock.hpp"

J3DMaterialFactoryV2::J3DMaterialFactoryV2(J3DMaterialBlockV2* srcBlock, bStream::CStream* stream) {
	mBlock = srcBlock;

	mInstanceTable.reserve(mBlock->Count + 1);

	stream->seek(mBlock->InstanceTableOffset);
	for (int i = 0; i < mBlock->Count; i++)
		mInstanceTable.push_back(stream->readUInt16());

	stream->seek(mBlock->NameTableOffset);
	mNameTable.Deserialize(stream);
}

std::shared_ptr<J3DMaterial> J3DMaterialFactoryV2::Create(bStream::CStream* stream, uint32_t index) {
	std::shared_ptr<J3DMaterial> newMaterial = std::make_shared<J3DMaterial>();
	newMaterial->Name = mNameTable.GetName(index);

	// Material init data can be "instanced", meaning that multiple materials use the same init data to load from file.
	// We can know which init data to use by looking up the proper index from the instance table.
	uint16_t instanceIndex = mInstanceTable[index];

	stream->seek(mBlock->InitDataTableOffset + (instanceIndex * sizeof(J3DMaterialInitDataV2)));

	J3DMaterialInitDataV2 initData;
	initData.Deserialize(stream);

	newMaterial->PEMode = (EPixelEngineMode)(initData.PEMode & 0x07);

	// Pixel engine block
	newMaterial->PEBlock.mZMode = ReadMaterialComponent<J3DZMode>(stream, mBlock->ZModeTableOffset, initData.ZMode);
	newMaterial->PEBlock.mAlphaCompare = ReadMaterialComponent<J3DAlphaCompare>(stream, mBlock->AlphaCompareTableOffset, initData.AlphaCompare);
	newMaterial->PEBlock.mBlendMode = ReadMaterialComponent<J3DBlendMode>(stream, mBlock->BlendInfoTableOffset, initData.BlendMode);

	if (initData.Fog != UINT16_MAX) {
		newMaterial->PEBlock.mFog = ReadMaterialComponent<J3DFog>(stream, mBlock->FogTableOffset, initData.Fog);
	}
	
	newMaterial->PEBlock.mZCompLoc = stream->peekUInt8(mBlock->ZCompLocTableOffset + initData.ZCompLoc * sizeof(uint8_t));
	newMaterial->PEBlock.mDither = stream->peekUInt8(mBlock->DitherTableOffset + initData.Dither * sizeof(uint8_t));

	// Light block
	newMaterial->LightBlock.mCullMode = (EGXCullMode)stream->peekUInt32(mBlock->CullModeTableOffset + initData.CullMode * sizeof(uint32_t));
	
	for (int i = 0; i < 2; i++) {
		if (initData.MaterialColor[i] != UINT16_MAX) {
			newMaterial->LightBlock.mMaterialColor[i].r = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MaterialColor[i] * sizeof(glm::uint32_t));
			newMaterial->LightBlock.mMaterialColor[i].g = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MaterialColor[i] * sizeof(glm::uint32_t) + 1);
			newMaterial->LightBlock.mMaterialColor[i].b = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MaterialColor[i] * sizeof(glm::uint32_t) + 2);
			newMaterial->LightBlock.mMaterialColor[i].a = stream->peekUInt8(mBlock->MaterialColorTableOffset + initData.MaterialColor[i] * sizeof(glm::uint32_t) + 3);

			newMaterial->LightBlock.mMaterialColor[i].r = newMaterial->LightBlock.mMaterialColor[i].r / 255.0f;
			newMaterial->LightBlock.mMaterialColor[i].g = newMaterial->LightBlock.mMaterialColor[i].g / 255.0f;
			newMaterial->LightBlock.mMaterialColor[i].b = newMaterial->LightBlock.mMaterialColor[i].b / 255.0f;
			newMaterial->LightBlock.mMaterialColor[i].a = newMaterial->LightBlock.mMaterialColor[i].a / 255.0f;
		}

		newMaterial->LightBlock.mAmbientColor[0] = glm::one<glm::vec4>();
		newMaterial->LightBlock.mAmbientColor[1] = glm::one<glm::vec4>();
	}

	uint8_t chanControlNum = stream->peekUInt8(mBlock->ColorChannelCountTableOffset + initData.ColorChannelCount * sizeof(uint8_t));
	for (int i = 0; i < chanControlNum * 2; i++) {
		newMaterial->LightBlock.mColorChannels.push_back(std::make_shared<J3DColorChannel>(ReadMaterialComponent<J3DColorChannel>(stream, mBlock->ColorChannelTableOffset, initData.ColorChannel[i])));
	}

	// Tex gen block
	uint8_t texGenNum = stream->peekUInt8(mBlock->TexGenCountTableOffset + initData.TexGenCount * sizeof(uint8_t));
	for (int i = 0; i < texGenNum; i++) {
		newMaterial->TexGenBlock.mTexCoordInfo.push_back(std::make_shared<J3DTexCoordInfo>(ReadMaterialComponent<J3DTexCoordInfo>(stream, mBlock->TexCoordTableOffset, initData.TexCoord[i])));

		if (initData.TexCoord2[i] != UINT16_MAX)
			newMaterial->TexGenBlock.mTexCoordInfo.push_back(std::make_shared<J3DTexCoordInfo>(ReadMaterialComponent<J3DTexCoordInfo>(stream, mBlock->TexCoord2TableOffset, initData.TexCoord2[i])));
	}

	for (int i = 0; i < 10; i++) {
		if (initData.TexMatrix[i] == UINT16_MAX)
			continue;

		newMaterial->TexGenBlock.mTexMatrix.push_back(std::make_shared<J3DTexMatrixInfo>(ReadMaterialComponent<J3DTexMatrixInfo>(stream, mBlock->TexMatrixTableOffset, initData.TexMatrix[i])));
	}

	if (initData.NBTScale != UINT16_MAX) {
		newMaterial->TexGenBlock.mNBTScale = ReadMaterialComponent<J3DNBTScaleInfo>(stream, mBlock->NBTScaleTableOffset, initData.NBTScale);
	}

	// Tev block
	for (int i = 0; i < 8; i++) {
		if (initData.TextureIndex[i] == UINT16_MAX)
			continue;

		newMaterial->TevBlock->mTextureIndices.push_back(stream->peekUInt16(mBlock->TextureIndexTableOffset + initData.TextureIndex[i] * sizeof(uint16_t)));
	}

	uint8_t tevStageNum = stream->peekUInt8(mBlock->TevStageCountTableOffset + initData.TEVStageCount * sizeof(uint8_t));
	newMaterial->TEVStageGenMax = tevStageNum;
	for (int i = 0; i < tevStageNum; i++) {
		if (initData.TEVOrder[i] != UINT16_MAX)
			newMaterial->TevBlock->mTevOrders.push_back(std::make_shared<J3DTevOrderInfo>(ReadMaterialComponent<J3DTevOrderInfo>(stream, mBlock->TevOrderTableOffset, initData.TEVOrder[i])));

		uint32_t currentOffset = stream->tell();
		if (initData.TEVSwapMode[i] != UINT16_MAX) {
			stream->seek(mBlock->TevSwapModeTableOffset + (initData.TEVSwapMode[i] * 4));
			J3DSwapModeInfo info;
			info.Deserialize(stream);

			stream->seek(mBlock->TevSwapTableOffset + initData.TEVSwapModeTable[info.TexIndex] * 4);
			newMaterial->TevBlock->mTevOrders[i]->mTexSwapMode.Deserialize(stream);

			stream->seek(mBlock->TevSwapTableOffset + initData.TEVSwapModeTable[info.RasIndex] * 4);
			newMaterial->TevBlock->mTevOrders[i]->mRasSwapMode.Deserialize(stream);
		}
		stream->seek(currentOffset);

		if (initData.TEVStage[i] != UINT16_MAX)
			newMaterial->TevBlock->mTevStages.push_back(std::make_shared<J3DTevStageInfo>(ReadMaterialComponent<J3DTevStageInfo>(stream, mBlock->TevStageTableOffset, initData.TEVStage[i])));
	}

	for (int i = 0; i < 16; i++) {
		newMaterial->TevBlock->mKonstColorSelection[i] = (EGXKonstColorSel)initData.TEVKonstColorSelect[i];
		newMaterial->TevBlock->mKonstAlphaSelection[i] = (EGXKonstAlphaSel)initData.TEVKonstAlphaSelect[i];
	}

	for (int i = 0; i < 4; i++) {
		if (initData.TEVColor[i] != UINT16_MAX) {
			newMaterial->TevBlock->mTevColors[i].r = stream->peekInt16(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::i16vec4));
			newMaterial->TevBlock->mTevColors[i].g = stream->peekInt16(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::i16vec4) + 2);
			newMaterial->TevBlock->mTevColors[i].b = stream->peekInt16(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::i16vec4) + 4);
			newMaterial->TevBlock->mTevColors[i].a = stream->peekInt16(mBlock->TevColorTableOffset + initData.TEVColor[i] * sizeof(glm::i16vec4) + 6);

			newMaterial->AnimationRegisterColors[i] = newMaterial->TevBlock->mTevColors[i];
		}

		if (initData.TEVKonstColor[i] != UINT16_MAX) {
			newMaterial->TevBlock->mTevKonstColors[i].r = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t));
			newMaterial->TevBlock->mTevKonstColors[i].g = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t) + 1);
			newMaterial->TevBlock->mTevKonstColors[i].b = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t) + 2);
			newMaterial->TevBlock->mTevKonstColors[i].a = stream->peekUInt8(mBlock->TevKColorTableOffset + initData.TEVKonstColor[i] * sizeof(uint32_t) + 3);

			newMaterial->TevBlock->mTevKonstColors[i].r = newMaterial->TevBlock->mTevKonstColors[i].r / 255.0f;
			newMaterial->TevBlock->mTevKonstColors[i].g = newMaterial->TevBlock->mTevKonstColors[i].g / 255.0f;
			newMaterial->TevBlock->mTevKonstColors[i].b = newMaterial->TevBlock->mTevKonstColors[i].b / 255.0f;
			newMaterial->TevBlock->mTevKonstColors[i].a = newMaterial->TevBlock->mTevKonstColors[i].a / 255.0f;

			newMaterial->AnimationKonstColors[i] = newMaterial->TevBlock->mTevKonstColors[i];
		}
	}

	newMaterial->LightBlock.mColorChannels.shrink_to_fit();
	newMaterial->TexGenBlock.mTexCoordInfo.shrink_to_fit();
	newMaterial->TexGenBlock.mTexCoord2Info.shrink_to_fit();
	newMaterial->TexGenBlock.mTexMatrix.shrink_to_fit();
	newMaterial->TevBlock->mTextureIndices.shrink_to_fit();
	newMaterial->TevBlock->mTevOrders.shrink_to_fit();
	newMaterial->TevBlock->mTevStages.shrink_to_fit();

	return newMaterial;
}

void J3DMaterialInitDataV2::Deserialize(bStream::CStream* stream) {
	PEMode = stream->readUInt8();            // 0x0000
	CullMode = stream->readUInt8();          // 0x0001
	ColorChannelCount = stream->readUInt8(); // 0x0002
	TexGenCount = stream->readUInt8();       // 0x0003
	TEVStageCount = stream->readUInt8();     // 0x0004
	ZCompLoc = stream->readUInt8();          // 0x0005
	ZMode = stream->readUInt8();             // 0x0006
	Dither = stream->readUInt8();            // 0x0007

	MaterialColor[0] = stream->readUInt16(); // 0x0008
	MaterialColor[1] = stream->readUInt16(); // 0x000A

	for (int i = 0; i < 4; i++)
		ColorChannel[i] = stream->readUInt16(); // 0x000C to 0x0012

	for (int i = 0; i < 8; i++)
		TexCoord[i] = stream->readUInt16();     // 0x0014 to 0x0022
	for (int i = 0; i < 8; i++)
		TexCoord2[i] = stream->readUInt16();    // 0x0024 to 0x0032

	for (int i = 0; i < 10; i++)
		TexMatrix[i] = stream->readUInt16();    // 0x0034 to 0x0046
	for (int i = 0; i < 20; i++)
		PostTexMatrix[i] = stream->readUInt16(); // 0x0048 to 0x006E

	for (int i = 0; i < 8; i++)
		TextureIndex[i] = stream->readUInt16(); // 0x0070 to 0x007E

	for (int i = 0; i < 4; i++)
		TEVKonstColor[i] = stream->readUInt16(); // 0x0080 to 0x0086

	size_t s = stream->tell();

	for (int i = 0; i < 16; i++)
		TEVKonstColorSelect[i] = stream->readUInt8(); // 0x0088 to 0x0097
	for (int i = 0; i < 16; i++)
		TEVKonstAlphaSelect[i] = stream->readUInt8(); // 0x0098 to 0x00A7

	for (int i = 0; i < 16; i++)
		TEVOrder[i] = stream->readUInt16(); // 0x00A8 to 0x00C7
	for (int i = 0; i < 4; i++)
		TEVColor[i] = stream->readUInt16(); // 0x00C8 to 0x00CF
	for (int i = 0; i < 16; i++)
		TEVStage[i] = stream->readUInt16(); // 0x00D0 to 0x00EF
	for (int i = 0; i < 16; i++)
		TEVSwapMode[i] = stream->readUInt16(); // 0x00F0 to 0x010F
	for (int i = 0; i < 16; i++)
		TEVSwapModeTable[i] = stream->readUInt16(); // 0x0110 to 0x012F

	Fog = stream->readUInt16();          // 0x0130
	AlphaCompare = stream->readUInt16(); // 0x0132
	BlendMode = stream->readUInt16();    // 0x0134
	NBTScale = stream->readUInt16();     // 0x0138
}