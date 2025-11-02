#include "J3D/Data/J3DData.hpp"
#include "J3D/Data/J3DBlock.hpp"
#include "J3D/Data/J3DModelData.hpp"

#include "J3D/Material/J3DMaterialTableLoader.hpp"
#include "J3D/Material/J3DMaterialTable.hpp"
#include "J3D/Material/J3DMaterialFactoryV2.hpp"
#include "J3D/Material/J3DMaterialFactoryV3.hpp"
#include "J3D/Material/J3DUniformBufferObject.hpp"

#include "J3D/Texture/J3DTexture.hpp"
#include "J3D/Texture/J3DTextureFactory.hpp"

#include "J3D/Util/J3DUtil.hpp"

#include <bstream.h>
#include <magic_enum/magic_enum.hpp>

#include <iostream>

J3DMaterialTableLoader::J3DMaterialTableLoader() {

}

std::shared_ptr<J3DMaterialTable> J3DMaterialTableLoader::Load(bStream::CStream* stream, std::shared_ptr<J3DModelData> modelData) {
	std::shared_ptr<J3DMaterialTable> materialTable = std::make_shared<J3DMaterialTable>();

	J3DDataBase header;
	header.Deserialize(stream);

	if (header.FormatVersion != EJ3DFormatVersion::BMT3) {
		// magic_enum breaks when enum values are beyond min/max, so we build our own name map for now
		static const std::unordered_map<EJ3DFormatVersion, std::string> formatNames = {
			{EJ3DFormatVersion::BCA1, "BCA1"}, // Joint animation with transforms for every frame
			{EJ3DFormatVersion::BCK1, "BCK1"}, // Joint animation with keyed transforms
			{EJ3DFormatVersion::BDL4, "BDL4"}, // Model with pre-compiled material display lists
			{EJ3DFormatVersion::BLA1, "BLA1"}, // Cluster animation with transforms for every frame
			{EJ3DFormatVersion::BLK1, "BLK1"}, // Cluster animation with keyed transforms
			{EJ3DFormatVersion::BMD1, "BMD1"}, // Model, version 1
			{EJ3DFormatVersion::BMD2, "BMD2"}, // Model, version 2
			{EJ3DFormatVersion::BMD3, "BMD3"}, // Model, version 3
			{EJ3DFormatVersion::BMT3, "BMT3"}, // External material data, version 3
			{EJ3DFormatVersion::BPK1, "BPK1"}, // Color animation with keyed colors
			{EJ3DFormatVersion::BRK1, "BRK1"}, // TEV register animation with keyed colors
			{EJ3DFormatVersion::BTK1, "BTK1"}, // Texture animation with keyed transforms
			{EJ3DFormatVersion::BTP1, "BTP1"}, // Texture flipbook animation
			{EJ3DFormatVersion::BXA1, "BXA1"}, // Vertex color animation with colors for every frame
			{EJ3DFormatVersion::BXK1, "BXK1"}  // Vertex color animation with keyed colors
		};
		std::cout << "Unknown BMT format version " << formatNames.at(header.FormatVersion) << "!" << std::endl;
	}

	for (uint32_t i = 0; i < header.BlockCount; i++) {
		switch ((EJ3DBlockType)stream->peekUInt32(stream->tell())) {
		case EJ3DBlockType::MAT2:
			ReadMaterialBlockV2(stream, materialTable);
			break;
		case EJ3DBlockType::MAT3:
			ReadMaterialBlockV3(stream, materialTable);
			break;
		case EJ3DBlockType::TEX1:
			ReadTextureBlock(stream, materialTable);
			break;
		default:
			uint32_t unsupportedBlockSize = stream->peekUInt32(stream->tell() + 4);
			stream->seek(stream->tell() + unsupportedBlockSize);
			break;
		}
	}

	// Initialize the materials that are being overridden from the model,
	// and add the materials NOT being overridden to the list.
	for (std::shared_ptr<J3DMaterial> defaultMaterial : modelData->GetMaterials()) {
		std::shared_ptr<J3DMaterial> instanceMaterial = materialTable->GetMaterial(defaultMaterial->Name);

		if (instanceMaterial != nullptr) {
			instanceMaterial->SetShape(defaultMaterial->GetShape());
			instanceMaterial->GenerateShaders();

			J3DUniformBufferObject::LinkMaterialToUBO(instanceMaterial);
		}
		else {
			materialTable->mMaterials.push_back(defaultMaterial);
		}
	}

	// Prune materials that weren't initialized.
	for (auto it = materialTable->mMaterials.begin(); it != materialTable->mMaterials.end(); ) {
		if ((*it)->GetShape().expired()) {
			it = materialTable->mMaterials.erase(it);

			continue;
		}

		++it;
	}

	// Copy the textures that aren't being overridden from the model.
	for (uint32_t i = 0; i < modelData->GetTextures().size(); i++) {
		if (i >= materialTable->mTextures.size()) {
			materialTable->mTextures.push_back(modelData->GetTexture(i));
		}
	}

	return materialTable;
}

void J3DMaterialTableLoader::ReadMaterialBlockV2(bStream::CStream* stream, std::shared_ptr<J3DMaterialTable> materialTable) {
	size_t currentStreamPos = stream->tell();

	J3DMaterialBlockV2 matBlock;
	matBlock.Deserialize(stream);

	J3DMaterialFactoryV2 materialFactory(&matBlock, stream);
	for (int i = 0; i < matBlock.Count; i++) {
		materialTable->mMaterials.push_back(materialFactory.Create(stream, i));
	}

	stream->seek(currentStreamPos + matBlock.BlockSize);
}

void J3DMaterialTableLoader::ReadMaterialBlockV3(bStream::CStream* stream, std::shared_ptr<J3DMaterialTable> materialTable) {
	size_t currentStreamPos = stream->tell();

	J3DMaterialBlockV3 matBlock;
	matBlock.Deserialize(stream);

	J3DMaterialFactoryV3 materialFactory(&matBlock, stream);
	for (int i = 0; i < matBlock.Count; i++) {
		materialTable->mMaterials.push_back(materialFactory.Create(stream, i));
	}

	stream->seek(currentStreamPos + matBlock.BlockSize);
}

void J3DMaterialTableLoader::ReadTextureBlock(bStream::CStream* stream, std::shared_ptr<J3DMaterialTable> materialTable) {
	size_t currentStreamPos = stream->tell();

	J3DTextureBlock texBlock;
	texBlock.Deserialize(stream);

	J3DTextureFactory textureFactory(&texBlock, stream);
	for (int i = 0; i < texBlock.Count; i++) {
		materialTable->mTextures.push_back(textureFactory.Create(stream, i));
	}

	stream->seek(currentStreamPos + texBlock.BlockSize);
}
