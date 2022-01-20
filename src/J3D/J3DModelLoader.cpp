#include "J3D/J3DModelLoader.hpp"
#include "J3D/J3DModelData.hpp"
#include "GX/GXEnum.hpp"
#include "GX/GXStruct.hpp"
#include "J3D/J3DJoint.hpp"
#include "J3D/J3DNameTable.hpp"
#include "J3D/J3DShapeFactory.hpp"
#include "J3D/J3DMaterialFactory.hpp"
#include "J3D/J3DJoint.hpp"

#include <bstream.h>

J3DModelLoader::J3DModelLoader() : mModelData(nullptr) {

}

J3DModelData* J3DModelLoader::Load(bStream::CStream* stream, uint32_t flags) {
    mModelData = new J3DModelData();
    
    J3DDataBase header;
    header.Deserialize(stream);

    for (int i = 0; i < header.BlockCount; i++) {
        switch ((EJ3DBlockType)stream->peekUInt32(stream->tell())) {
            case EJ3DBlockType::INF1:
                ReadInformationBlock(stream, flags);
                break;
            case EJ3DBlockType::VTX1:
                ReadVertexBlock(stream, flags);
                break;
            case EJ3DBlockType::EVP1:
                ReadEnvelopeBlock(stream, flags);
                break;
            case EJ3DBlockType::DRW1:
                ReadDrawBlock(stream, flags);
                break;
            case EJ3DBlockType::JNT1:
                ReadJointBlock(stream, flags);
                break;
            case EJ3DBlockType::SHP1:
                ReadShapeBlock(stream, flags);
                break;
            case EJ3DBlockType::MAT3:
                ReadMaterialBlock(stream, flags);
                break;
        }
    }

    uint32_t index = 0;
    mModelData->MakeHierarchy(nullptr, index);

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

    while ((EGXAttribute)stream->peekUInt32(stream->tell()) != EGXAttribute::Null) {
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

void J3DModelLoader::ReadEnvelopeBlock(bStream::CStream* stream, uint32_t flags) {
    size_t currentStreamPos = stream->tell();

    J3DEnvelopeBlock envBlock;
    envBlock.Deserialize(stream);

    uint8_t runningWeightIndex = 0;

    for (int i = 0; i < envBlock.Count; i++) {
        J3DEnvelope envelope;

        uint8_t curJointCount = stream->readUInt8();

        // Go to the joint index data and read this envelope's joint indices
        stream->seek(envBlock.EnvelopeIndexTableOffset + (runningWeightIndex * sizeof(uint16_t)));
        for (int j = 0; j < curJointCount; j++)
            envelope.JointIndices.push_back(stream->readUInt16());

        // Go to the weight data and read this joint's weights
        stream->seek(envBlock.WeightTableOffset + (runningWeightIndex * sizeof(float)));
        for (int j = 0; j < curJointCount; j++)
            envelope.Weights.push_back(stream->readFloat());

        mModelData->mJointEnvelopes.push_back(envelope);
        stream->seek(envBlock.JointIndexTableOffset + (i + sizeof(uint8_t)));

        runningWeightIndex += curJointCount;
    }

    stream->seek(envBlock.MatrixTableOffset);

    // Read the joints' inverse bind matrices
    uint32_t matrixCount = ((envBlock.BlockOffset + envBlock.BlockSize) - envBlock.MatrixTableOffset) / sizeof(glm::mat4x3);
    for (int i = 0; i < matrixCount; i++) {
        glm::mat4 matrix;

        matrix[3][0] = 0.f;
        matrix[3][1] = 0.f;
        matrix[3][2] = 0.f;
        matrix[3][3] = 1.f;

        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 4; y++) {
                matrix[x][y] = stream->readFloat();
            }
        }

        mModelData->mInverseBindMatrices.push_back(glm::transpose(matrix));
    }

    stream->seek(currentStreamPos + envBlock.BlockSize);
}

void J3DModelLoader::ReadDrawBlock(bStream::CStream* stream, uint32_t flags) {
    size_t currentStreamPos = stream->tell();

    J3DDrawBlock drawBlock;
    drawBlock.Deserialize(stream);

    stream->seek(drawBlock.DrawTableOffset);

    for (int i = 0; i < drawBlock.Count; i++) {
        mModelData->mDrawBools.push_back(stream->readUInt8());
        stream->seek(drawBlock.IndexTableOffset + (i * sizeof(uint16_t)));

        mModelData->mEnvelopeIndices.push_back(stream->readUInt16());
        stream->seek(drawBlock.DrawTableOffset + (i * sizeof(uint8_t)));
    }

    stream->seek(currentStreamPos + drawBlock.BlockSize);
}

void J3DModelLoader::ReadJointBlock(bStream::CStream* stream, uint32_t flags) {
    size_t currentStreamPos = stream->tell();

    J3DJointBlock jointBlock;
    jointBlock.Deserialize(stream);

    stream->seek(jointBlock.NameTableOffset);

    J3DNameTable nameTable;
    nameTable.Deserialize(stream);

    stream->seek(jointBlock.InitDataTableOffset);
    for (int i = 0; i < jointBlock.Count; i++) {
        J3DJoint* newJoint = new J3DJoint();
        
        newJoint->mJointName = nameTable.GetName(i);
        newJoint->mJointID = i;

        newJoint->mMatrixFlag = stream->readUInt16();
        newJoint->mAttachFlag = stream->readUInt8();

        stream->skip(1);

        newJoint->mTransform.Deserialize(stream);

        newJoint->mBoundingSphereRadius = stream->readFloat();

        newJoint->mBoundingBoxMin.x = stream->readFloat();
        newJoint->mBoundingBoxMin.y = stream->readFloat();
        newJoint->mBoundingBoxMin.z = stream->readFloat();

        newJoint->mBoundingBoxMax.x = stream->readFloat();
        newJoint->mBoundingBoxMax.y = stream->readFloat();
        newJoint->mBoundingBoxMax.z = stream->readFloat();

        mModelData->mJoints.push_back(newJoint);
    }

    stream->seek(currentStreamPos + jointBlock.BlockSize);
}

void J3DModelLoader::ReadShapeBlock(bStream::CStream* stream, uint32_t flags) {
    size_t currentStreamPos = stream->tell();

    J3DShapeBlock shapeBlock;
    shapeBlock.Deserialize(stream);

    J3DShapeFactory shapeFactory(&shapeBlock);
    for (int i = 0; i < shapeBlock.Count; i++) {
        mModelData->mShapes.push_back(shapeFactory.Create(stream, i));
    }

    stream->seek(currentStreamPos + shapeBlock.BlockSize);
}

void J3DModelLoader::ReadMaterialBlock(bStream::CStream* stream, uint32_t flags) {
    size_t currentStreamPos = stream->tell();

    J3DMaterialBlock matBlock;
    matBlock.Deserialize(stream);

    J3DMaterialFactory materialFactory(&matBlock, stream);
    for (int i = 0; i < matBlock.Count; i++) {
        mModelData->mMaterials.push_back(materialFactory.Create(stream, i));
    }

    stream->seek(currentStreamPos + matBlock.BlockSize);
}
