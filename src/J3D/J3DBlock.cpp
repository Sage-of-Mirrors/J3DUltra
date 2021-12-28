#include "J3D/J3DBlock.hpp"
#include "J3D/J3DVertexData.hpp"
#include "../lib/bStream/bstream.h"
#include "GX/GXEnum.hpp"

bool J3DBlock::Deserialize(bStream::CStream* stream) {
    try {
        BlockOffset = stream->tell();
        BlockType = (EJ3DBlockType)stream->readUInt32();
        BlockSize = stream->readUInt32();
    }
    catch(...) {
        return false;
    }

    return true;
}

bool J3DModelInfoBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream))
        return false;

    try {
        Flags = stream->readUInt16();
        MatrixGroupCount = stream->readUInt32();
        VertexPositionCount = stream->readUInt32();

        HierarchyOffset = stream->readUInt32() + currentStreamPosition;
    }
    catch (...) {
        return false;
    }

    return true;
}

bool J3DVertexBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream))
        return false;

    try {
        AttributeTableOffset = stream->readUInt32() + currentStreamPosition;

        PositionTableOffset = stream->readUInt32();
        if (PositionTableOffset != 0)
            PositionTableOffset += currentStreamPosition;

        NormalTableOffset = stream->readUInt32();
        if (NormalTableOffset != 0)
            NormalTableOffset += currentStreamPosition;

        NBTTableOffset = stream->readUInt32();
        if (NBTTableOffset != 0)
            NBTTableOffset += currentStreamPosition;

        ColorTablesOffset[0] = stream->readUInt32();
        if (ColorTablesOffset[0] != 0)
            ColorTablesOffset[0] += currentStreamPosition;

        ColorTablesOffset[1] = stream->readUInt32();
        if (ColorTablesOffset[1] != 0)
            ColorTablesOffset[1] += currentStreamPosition;

        for (int i = 0; i < 8; i++) {
            TexCoordTablesOffset[i] = stream->readUInt32();
            if (TexCoordTablesOffset[i] != 0)
                TexCoordTablesOffset[i] += currentStreamPosition;
        }
    }
    catch (...) {
        return false;
    }

    return true;
}

uint32_t J3DVertexBlock::GetAttributeElementCount(GXVertexAttributeList& curAttribute) {
    uint32_t elementCount = 0;
    
    switch (curAttribute.Attribute) {
        case EGXAttribute::Position:
        case EGXAttribute::Normal:
            elementCount = 3;
            break;
        case EGXAttribute::Color0:
        case EGXAttribute::Color1:
            switch (curAttribute.ComponentType) {
                case EGXComponentType::RGB565:
                case EGXComponentType::RGBA4:
                    elementCount = 2;
                    break;
                case EGXComponentType::RGB8:
                case EGXComponentType::RGBA6:
                    elementCount = 3;
                    break;
                case EGXComponentType::RGBX8:
                case EGXComponentType::RGBA8:
                    elementCount = 4;
                    break;
            }
            break;
        case EGXAttribute::TexCoord0:
        case EGXAttribute::TexCoord1:
        case EGXAttribute::TexCoord2:
        case EGXAttribute::TexCoord3:
        case EGXAttribute::TexCoord4:
        case EGXAttribute::TexCoord5:
        case EGXAttribute::TexCoord6:
        case EGXAttribute::TexCoord7:
            elementCount = 2;
            break;
        default:
            break;
    }

    return elementCount;
}

void J3DVertexBlock::LoadAttributeData(J3DVertexData* vertexData, bStream::CStream* stream, GXVertexAttributeList& curAttribute, GXVertexAttributeList& nextAttribute) {
    uint32_t attributeCount = CalculateAttributeCount(curAttribute, nextAttribute);
    uint32_t elementCount = GetAttributeElementCount(curAttribute);

    //std::vector<float> floatBuffer;
    float scaleFactor = powf(0.5f, curAttribute.Fraction);

    for (int i = 0; i < attributeCount; i++) {
        std::vector<float> floatBuffer;

        for (int j = 0; j < elementCount; j++) {
            if (curAttribute.Attribute == EGXAttribute::Color0 || curAttribute.Attribute == EGXAttribute::Color1) {
                switch (curAttribute.ComponentType) {
                    case EGXComponentType::RGB8:
                    case EGXComponentType::RGBA8:
                    case EGXComponentType::RGBX8:
                        floatBuffer.push_back(stream->readUInt8() / 255.f);
                        break;
                    default:
                        floatBuffer.push_back(0.f);
                        break;
                }
            }
            else {
                switch (curAttribute.ComponentType) {
                    case EGXComponentType::Float:
                        floatBuffer.push_back(stream->readFloat() * scaleFactor);
                        break;
                    case EGXComponentType::Unsigned16:
                        floatBuffer.push_back(stream->readUInt16() * scaleFactor);
                        break;
                    case EGXComponentType::Signed16:
                        floatBuffer.push_back(stream->readInt16() * scaleFactor);
                        break;
                    case EGXComponentType::Unsigned8:
                        floatBuffer.push_back(stream->readUInt8() * scaleFactor);
                        break;
                    case EGXComponentType::Signed8:
                        floatBuffer.push_back(stream->readInt8() * scaleFactor);
                        break;
                    default:
                        floatBuffer.push_back(0.f);
                        break;
                }
            }
        }

        switch (curAttribute.Attribute) {
            case EGXAttribute::Position:
                vertexData->Positions.push_back(glm::vec3(floatBuffer[0], floatBuffer[1], floatBuffer[2]));
                break;
            case EGXAttribute::Normal:
                vertexData->Normals.push_back(glm::vec3(floatBuffer[0], floatBuffer[1], floatBuffer[2]));
                break;
            case EGXAttribute::Color0:
            case EGXAttribute::Color1:
            {
                uint32_t colorIndex = (uint32_t)curAttribute.Attribute - (uint32_t)EGXAttribute::Color0;

                if (curAttribute.ComponentType == EGXComponentType::RGB8)
                    vertexData->Colors[colorIndex].push_back(glm::vec4(floatBuffer[0], floatBuffer[1], floatBuffer[2], 1.f));
                else
                    vertexData->Colors[colorIndex].push_back(glm::vec4(floatBuffer[0], floatBuffer[1], floatBuffer[2], floatBuffer[3]));

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
                uint32_t texCoordIndex = (uint32_t)curAttribute.Attribute - (uint32_t)EGXAttribute::TexCoord0;
                vertexData->TexCoords[texCoordIndex].push_back(glm::vec2(floatBuffer[0], floatBuffer[1]));

                break;
            }
        }
    }
}

uint32_t J3DVertexBlock::CalculateAttributeCount(GXVertexAttributeList& curAttribute, GXVertexAttributeList& nextAttribute) {
    uint32_t elementSize = 0, currentAttributeOffset = 0, nextAttributeOffset = 0;

    // First check the attribute to get the number of elements. We ignore colors because they're treated differently.
    switch (curAttribute.Attribute) {
        case EGXAttribute::Position:
            elementSize = 3;
            currentAttributeOffset = PositionTableOffset;
            break;
        case EGXAttribute::Normal:
            elementSize = 3;
            currentAttributeOffset = NormalTableOffset;
            break;
        case EGXAttribute::Color0:
        case EGXAttribute::Color1:
            currentAttributeOffset = ColorTablesOffset[(uint32_t)curAttribute.Attribute - (uint32_t)EGXAttribute::Color0];
            break;
        case EGXAttribute::TexCoord0:
        case EGXAttribute::TexCoord1:
        case EGXAttribute::TexCoord2:
        case EGXAttribute::TexCoord3:
        case EGXAttribute::TexCoord4:
        case EGXAttribute::TexCoord5:
        case EGXAttribute::TexCoord6:
        case EGXAttribute::TexCoord7:
            elementSize = 2;
            currentAttributeOffset = TexCoordTablesOffset[(uint32_t)curAttribute.Attribute - (uint32_t)EGXAttribute::TexCoord0];
            break;
        default:
            break;
    }

    // Colors get their element count from the type rather than the attribute itself.
    if (curAttribute.Attribute == EGXAttribute::Color0 || curAttribute.Attribute == EGXAttribute::Color1) {
        switch (curAttribute.ComponentType) {
            case EGXComponentType::RGB565:
            case EGXComponentType::RGBA4:
                elementSize = 2;
                break;
            case EGXComponentType::RGB8:
            case EGXComponentType::RGBA6:
                elementSize = 3;
                break;
            case EGXComponentType::RGBX8:
            case EGXComponentType::RGBA8:
                elementSize = 4;
                break;
        }
    }
    // Everything else gets multiplied by what type it is.
    else {
        switch (curAttribute.ComponentType) {
            case EGXComponentType::Float:
                elementSize *= 4;
                break;
            case EGXComponentType::Unsigned16:
            case EGXComponentType::Signed16:
                elementSize *= 2;
                break;
            case EGXComponentType::Unsigned8:
            case EGXComponentType::Signed8:
                // Multiply by 1, we'll skip that
            default:
                break;
        }
    }

    // Figure out what our next attribute offset is. If the attribute is Null, we set the next offset to the end of the block.
    switch (nextAttribute.Attribute) {
        // We don't put Position here because position is always the first attribute.
        case EGXAttribute::Normal:
            nextAttributeOffset = NormalTableOffset;
            break;
        case EGXAttribute::Color0:
        case EGXAttribute::Color1:
            nextAttributeOffset = ColorTablesOffset[(uint32_t)nextAttribute.Attribute - (uint32_t)EGXAttribute::Color0];
            break;
        case EGXAttribute::TexCoord0:
        case EGXAttribute::TexCoord1:
        case EGXAttribute::TexCoord2:
        case EGXAttribute::TexCoord3:
        case EGXAttribute::TexCoord4:
        case EGXAttribute::TexCoord5:
        case EGXAttribute::TexCoord6:
        case EGXAttribute::TexCoord7:
            nextAttributeOffset = TexCoordTablesOffset[(uint32_t)nextAttribute.Attribute - (uint32_t)EGXAttribute::TexCoord0];
            break;
        case EGXAttribute::Null:
            nextAttributeOffset = BlockOffset + BlockSize;
            break;
    }

    return (nextAttributeOffset - currentAttributeOffset) / elementSize;
}

bool J3DEnvelopeBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream))
        return false;

    try {
        Count = stream->readUInt16();
        stream->skip(2);

        JointIndexTableOffset = stream->readUInt32() + currentStreamPosition;
        EnvelopeIndexTableOffset = stream->readUInt32() + currentStreamPosition;
        WeightTableOffset = stream->readUInt32() + currentStreamPosition;
        MatrixTableOffset = stream->readUInt32() + currentStreamPosition;
    }
    catch (...) {
        return false;
    }

    return true;
}

bool J3DDrawBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream))
        return false;

    try {
        Count = stream->readUInt16();
        stream->skip(2);

        DrawTableOffset = stream->readUInt32() + currentStreamPosition;
        IndexTableOffset = stream->readUInt32() + currentStreamPosition;
    }
    catch (...) {
        return false;
    }

    return true;
}

bool J3DJointBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream))
        return false;

    try {
        Count = stream->readUInt16();
        stream->skip(2);

        InitDataTableOffset = stream->readUInt32() + currentStreamPosition;
        IndexTableOffset = stream->readUInt32() + currentStreamPosition;
        NameTableOffset = stream->readUInt32() + currentStreamPosition;
    }
    catch (...) {
        return false;
    }

    return true;
}

bool J3DShapeBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream))
        return false;

    try {
        Count = stream->readUInt16();
        stream->skip(2);

        InitDataTableOffset = stream->readUInt32() + currentStreamPosition;
        IndexTableOffset = stream->readUInt32() + currentStreamPosition;
        NameTableOffset = stream->readUInt32() + currentStreamPosition;
        VertexDescriptorTableOffset = stream->readUInt32() + currentStreamPosition;
        MatrixTableOffset = stream->readUInt32() + currentStreamPosition;
        DrawTableOffset = stream->readUInt32() + currentStreamPosition;
        MatrixInitTableOffset = stream->readUInt32() + currentStreamPosition;
        DrawInitDataTableOffset = stream->readUInt32() + currentStreamPosition;
    }
    catch (...) {
        return false;
    }

    return true;
}
