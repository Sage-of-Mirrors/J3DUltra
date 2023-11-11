#include "J3D/Material/J3DMaterialTableLoader.hpp"
#include "J3D/Material/J3DMaterialTable.hpp"
#include "J3D/Material/J3DMaterialFactoryV2.hpp"
#include "J3D/Material/J3DMaterialFactoryV3.hpp"
#include "J3D/Material/J3DUniformBufferObject.hpp"

#include "J3D/Texture/J3DTexture.hpp"
#include "J3D/Texture/J3DTextureFactory.hpp"

#include "J3D/Data/J3DData.hpp"
#include "J3D/Data/J3DBlock.hpp"
#include "J3D/Data/J3DModelData.hpp"

#include "J3D/Util/J3DUtil.hpp"

#include <bstream.h>
#include <magic_enum.hpp>

#include <iostream>

J3DMaterialTableLoader::J3DMaterialTableLoader() {

}

std::shared_ptr<J3DMaterialTable> J3DMaterialTableLoader::Load(bStream::CStream* stream, std::shared_ptr<J3DModelData> modelData) {
    mMaterialTable = std::make_shared<J3DMaterialTable>();

    J3DDataBase header;
    header.Deserialize(stream);

    if (header.FormatVersion != EJ3DFormatVersion::BMT3) {
        std::cout << "Unknown BMT format version " << magic_enum::enum_name(header.FormatVersion) << "!" << std::endl;
    }

    for (uint32_t i = 0; i < header.BlockCount; i++) {
        switch ((EJ3DBlockType)stream->peekUInt32(stream->tell())) {
        case EJ3DBlockType::MAT2:
            ReadMaterialBlockV2(stream);
            break;
        case EJ3DBlockType::MAT3:
            ReadMaterialBlockV3(stream);
            break;
        case EJ3DBlockType::TEX1:
            ReadTextureBlock(stream);
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
        std::shared_ptr<J3DMaterial> instanceMaterial = mMaterialTable->GetMaterial(defaultMaterial->Name);

        if (instanceMaterial != nullptr) {
            instanceMaterial->SetShape(defaultMaterial->GetShape());
            instanceMaterial->GenerateShaders();

            J3DUniformBufferObject::LinkMaterialToUBO(instanceMaterial);
        }
        else {
            mMaterialTable->mMaterials.push_back(defaultMaterial);
        }
    }

    // Prune materials that weren't initialized.
    for (auto it = mMaterialTable->mMaterials.begin(); it != mMaterialTable->mMaterials.end(); ) {
        if ((*it)->GetShape() == nullptr) {
            it = mMaterialTable->mMaterials.erase(it);

            continue;
        }

        ++it;
    }

    // Copy the textures that aren't being overridden from the model.
    for (uint32_t i = 0; i < modelData->GetTextures().size(); i++) {
        if (i >= mMaterialTable->mTextures.size()) {
            mMaterialTable->mTextures.push_back(modelData->GetTexture(i));
        }
    }

    return mMaterialTable;
}

void J3DMaterialTableLoader::ReadMaterialBlockV2(bStream::CStream* stream) {
    size_t currentStreamPos = stream->tell();

    J3DMaterialBlockV2 matBlock;
    matBlock.Deserialize(stream);

    J3DMaterialFactoryV2 materialFactory(&matBlock, stream);
    for (int i = 0; i < matBlock.Count; i++) {
        mMaterialTable->mMaterials.push_back(materialFactory.Create(stream, i));
    }

    stream->seek(currentStreamPos + matBlock.BlockSize);
}

void J3DMaterialTableLoader::ReadMaterialBlockV3(bStream::CStream* stream) {
    size_t currentStreamPos = stream->tell();

    J3DMaterialBlockV3 matBlock;
    matBlock.Deserialize(stream);

    J3DMaterialFactoryV3 materialFactory(&matBlock, stream);
    for (int i = 0; i < matBlock.Count; i++) {
        mMaterialTable->mMaterials.push_back(materialFactory.Create(stream, i));
    }

    stream->seek(currentStreamPos + matBlock.BlockSize);
}

void J3DMaterialTableLoader::ReadTextureBlock(bStream::CStream* stream) {
    size_t currentStreamPos = stream->tell();

    J3DTextureBlock texBlock;
    texBlock.Deserialize(stream);

    J3DTextureFactory textureFactory(&texBlock, stream);
    for (int i = 0; i < texBlock.Count; i++) {
        mMaterialTable->mTextures.push_back(textureFactory.Create(stream, i));
    }

    stream->seek(currentStreamPos + texBlock.BlockSize);
}
