#include "J3D/J3DModelLoader.hpp"
#include "J3D/J3DModelData.hpp"
#include "../lib/bStream/bstream.h"
#include "GX/GXEnum.hpp"
#include "GX/GXStruct.hpp"

J3DModelLoader::J3DModelLoader() : mModelData(nullptr) {

}

J3DModelData* J3DModelLoader::Load(bStream::CStream* stream, uint32_t flags) {
    mModelData = new J3DModelData();
    
    J3DDataBase header;
    header.Deserialize(stream);

    for (int i = 0; i < header.BlockCount; i++) {
        switch ((EJ3DBlockType)stream->peekUInt32(0)) {
            case EJ3DBlockType::INF1:
                ReadInformationBlock(stream, flags);
                break;
        }
    }

    mModelData->MakeHierarchy(nullptr, &mModelData->mHierarchyNodes);

    return mModelData;
}

void J3DModelLoader::ReadInformationBlock(bStream::CStream* stream, uint32_t flags) {
    size_t currentStreamPos = stream->tell();

    J3DModelInfoBlock infoBlock;
    infoBlock.Deserialize(stream);

    mModelData->mFlags = flags | infoBlock.Flags;

    // TODO: Create the matrix calc types and assign them here
    // Set up the matrix calculator this model should use - basic, Maya, or SoftImage.
    switch (mModelData->mFlags & FLAGS_MATRIX_MASK) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
    }

    mModelData->mMatrixGroupCount = infoBlock.MatrixGroupCount;
    //mModelData->mVertexData.PositionCount = infoBlock->VertexPositionCount;

    stream->seek(infoBlock.HierarchyOffset);

    J3DModelHierarchy hierarchyNode;
    hierarchyNode.Type = (EJ3DHierarchyType)stream->readUInt16();
    hierarchyNode.Index = stream->readUInt16();

    mModelData->mHierarchyNodes.push_back(hierarchyNode);

    while (hierarchyNode.Type != EJ3DHierarchyType::Exit) {
        hierarchyNode.Type = (EJ3DHierarchyType)stream->readUInt16();
        hierarchyNode.Index = stream->readUInt16();

        mModelData->mHierarchyNodes.push_back(hierarchyNode);
    }

    stream->seek(currentStreamPos + infoBlock.BlockSize);
}

void J3DModelLoader::ReadVertexBlock(bStream::CStream* stream, uint32_t flags) {
    size_t currentStreamPos = stream->tell();

    J3DVertexBlock vtxBlock;
    vtxBlock.Deserialize(stream);

    // Load the attribute data
    std::vector<GXVertexAttributeList> attributes;
    stream->seek(vtxBlock.AttributeTableOffset);

    while ((EGXAttribute)stream->peekUInt32(0) != EGXAttribute::Null) {
        GXVertexAttributeList attribute {
            (EGXAttribute)stream->readUInt32(),
            (EGXComponentCount)stream->readUInt32(),
            (EGXComponentType)stream->readUInt32(),
            stream->readUInt8()
        };

        attributes.push_back(attribute);

        stream->skip(3);
    }

    J3DVertexData* vtxData = &mModelData->mVertexData;

    // Now load the vertex data, converting from whatever format it's in to vec2/3/4
    for (auto it = attributes.begin(); it != attributes.end(); it++)
    {
        GXVertexAttributeList nextAttribute { EGXAttribute::Null };
        if (it + 1 != attributes.end())
            nextAttribute = *(it + 1);

        switch (it->Attribute) {
            case EGXAttribute::Position:
                stream->seek(vtxBlock.PositionTableOffset);
                vtxBlock.LoadAttributeData(vtxData, stream, *it, nextAttribute);
                break;
            case EGXAttribute::Normal:
                stream->seek(vtxBlock.NormalTableOffset);
                vtxBlock.LoadAttributeData(vtxData, stream, *it, nextAttribute);
                break;
            case EGXAttribute::Color0:
            case EGXAttribute::Color1:
            {
                uint32_t colorIndex = (uint32_t)it->Attribute - (uint32_t)EGXAttribute::Color0;

                stream->seek(vtxBlock.ColorTablesOffset[colorIndex]);
                vtxBlock.LoadAttributeData(vtxData, stream, *it, nextAttribute);
                break;
            }
            case EGXAttribute::TexCoord0:
            case EGXAttribute::TexCoord1:
            case EGXAttribute::TexCoord2:
            case EGXAttribute::TexCoord3:
            case EGXAttribute::TexCoord4:
            case EGXAttribute::TexCoord5:
            case EGXAttribute::TexCoord6:
            case EGXAttribute::TexCoord7:
            {
                uint32_t texCoordIndex = (uint32_t)it->Attribute - (uint32_t)EGXAttribute::TexCoord0;

                stream->seek(vtxBlock.TexCoordTablesOffset[texCoordIndex]);
                vtxBlock.LoadAttributeData(vtxData, stream, *it, nextAttribute);
                break;
            }
        }
    }
    
    stream->seek(currentStreamPos + vtxBlock.BlockSize);
}
