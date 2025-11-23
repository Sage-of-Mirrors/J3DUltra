#include "J3D/Data/J3DBlock.hpp"
#include "J3D/Geometry/J3DVertexData.hpp"

#include <GXVertexData.hpp>
#include <bstream.h>

bool J3DBlock::Deserialize(bStream::CStream* stream) {
    try {
        BlockOffset = (uint32_t)stream->tell();
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
        stream->skip(2);
        MatrixGroupCount = stream->readUInt32();
        VertexPositionCount = stream->readUInt32();

        HierarchyOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
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
    AttributeTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);

    PositionTableOffset = stream->readUInt32();
    if (PositionTableOffset != 0)
      PositionTableOffset += (uint32_t)currentStreamPosition;

    NormalTableOffset = stream->readUInt32();
    if (NormalTableOffset != 0)
      NormalTableOffset += (uint32_t)currentStreamPosition;

    NBTTableOffset = stream->readUInt32();
    if (NBTTableOffset != 0)
      NBTTableOffset += (uint32_t)currentStreamPosition;

    ColorTablesOffset[0] = stream->readUInt32();
    if (ColorTablesOffset[0] != 0)
      ColorTablesOffset[0] += (uint32_t)currentStreamPosition;

    ColorTablesOffset[1] = stream->readUInt32();
    if (ColorTablesOffset[1] != 0)
      ColorTablesOffset[1] += (uint32_t)currentStreamPosition;

    for (int i = 0; i < 8; i++) {
      TexCoordTablesOffset[i] = stream->readUInt32();
      if (TexCoordTablesOffset[i] != 0)
        TexCoordTablesOffset[i] += (uint32_t)currentStreamPosition;
    }
  }
  catch (...) {
    return false;
  }

  return true;
}

uint32_t J3DVertexBlock::GetAttributeElementCount(GXVertexAttributeFormat& curAttribute) {
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

void J3DVertexBlock::LoadAttributeData(GXAttributeData* vertexData, bStream::CStream* stream, GXVertexAttributeFormat& curAttribute, GXVertexAttributeFormat& nextAttribute) {
  uint32_t attributeCount = CalculateAttributeCount(curAttribute, nextAttribute);
  uint32_t elementCount = GetAttributeElementCount(curAttribute);

  float scaleFactor = powf(0.5f, curAttribute.FixedPoint);

  for (uint32_t i = 0; i < attributeCount; i++) {
    std::vector<float> floatBuffer;

    for (uint32_t j = 0; j < elementCount; j++) {
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
      vertexData->GetPositions().push_back(glm::vec4(floatBuffer[0], floatBuffer[1], floatBuffer[2], 0.f));
      break;
    case EGXAttribute::Normal:
      vertexData->GetNormals().push_back(glm::vec3(floatBuffer[0], floatBuffer[1], floatBuffer[2]));
      break;
    case EGXAttribute::Color0:
    case EGXAttribute::Color1:
    {
      uint32_t colorIndex = (uint32_t)curAttribute.Attribute - (uint32_t)EGXAttribute::Color0;

      if (curAttribute.ComponentType == EGXComponentType::RGB8)
        vertexData->GetColors(colorIndex).push_back(glm::vec4(floatBuffer[0], floatBuffer[1], floatBuffer[2], 1.f));
      else
        vertexData->GetColors(colorIndex).push_back(glm::vec4(floatBuffer[0], floatBuffer[1], floatBuffer[2], floatBuffer[3]));

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
      vertexData->GetTexCoords(texCoordIndex).push_back(glm::vec3(floatBuffer[0], floatBuffer[1], 0.f));

                break;
            }
        }
    }

    vertexData->GetPositions().shrink_to_fit();
    vertexData->GetNormals().shrink_to_fit();
    vertexData->GetColors(0).shrink_to_fit();
    vertexData->GetColors(1).shrink_to_fit();

    for (int texcoord = 0; texcoord < 8; texcoord++){
        vertexData->GetTexCoords(texcoord).shrink_to_fit();
    }
}

uint32_t J3DVertexBlock::CalculateAttributeCount(GXVertexAttributeFormat& curAttribute, GXVertexAttributeFormat& nextAttribute) {
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

    JointIndexTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    EnvelopeIndexTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    WeightTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    MatrixTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
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

    DrawTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    IndexTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
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

    InitDataTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    IndexTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    NameTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
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

    InitDataTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    IndexTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    NameTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    VertexDescriptorTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    MatrixTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    DrawTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    MatrixInitTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    DrawInitDataTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
  }
  catch (...) {
    return false;
  }

  return true;
}

bool J3DMaterialBlockV2::Deserialize(bStream::CStream* stream) {
  size_t currentStreamPosition = stream->tell();

  if (!J3DBlock::Deserialize(stream))
    return false;

  try {
    Count = stream->readUInt16();
    stream->skip(2);

    InitDataTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    InstanceTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    NameTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    CullModeTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    MaterialColorTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    ColorChannelCountTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    ColorChannelTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TexGenCountTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TexCoordTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TexCoord2TableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TexMatrixTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    PostTexMatrixTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TextureIndexTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevOrderTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevColorTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevKColorTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevStageCountTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevStageTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevSwapModeTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevSwapTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    FogTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    AlphaCompareTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    BlendInfoTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    ZModeTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    ZCompLocTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    DitherTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    NBTScaleTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
  }
  catch (...) {
    return false;
  }

  return true;
}

bool J3DMaterialBlockV3::Deserialize(bStream::CStream* stream) {
  size_t currentStreamPosition = stream->tell();

  if (!J3DBlock::Deserialize(stream))
    return false;

  try {
    Count = stream->readUInt16();
    stream->skip(2);

    InitDataTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    InstanceTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    NameTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    IndirectInitDataTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    CullModeTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    MaterialColorTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    ColorChannelCountTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    ColorChannelTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    AmbientColorTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    LightTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TexGenCountTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TexCoordTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TexCoord2TableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TexMatrixTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    PostTexMatrixTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TextureIndexTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevOrderTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevColorTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevKColorTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevStageCountTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevStageTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevSwapModeTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    TevSwapTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    FogTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    AlphaCompareTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    BlendInfoTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    ZModeTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    ZCompLocTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    DitherTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    NBTScaleTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
  }
  catch (...) {
    return false;
  }

  return true;
}

bool J3DTextureBlock::Deserialize(bStream::CStream* stream) {
  size_t currentStreamPosition = stream->tell();

  if (!J3DBlock::Deserialize(stream))
    return false;

  try {
    Count = stream->readUInt16();
    stream->skip(2);

    TexTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    NameTableOffset = (uint32_t)(stream->readUInt32() + currentStreamPosition);
    }
    catch (...) {
        return false;
    }

    return true;
}

bool J3DRegisterColorKeyBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream)) {
        return false;
    }

    try {
        LoopMode = stream->readUInt8();
        stream->skip(1);

        Length = stream->readUInt16();
        RegisterTrackCount = stream->readUInt16();
        KonstTrackCount = stream->readUInt16();

        RegisterRedCount = stream->readUInt16();
        RegisterGreenCount = stream->readUInt16();
        RegisterBlueCount = stream->readUInt16();
        RegisterAlphaCount = stream->readUInt16();

        KonstRedCount = stream->readUInt16();
        KonstGreenCount = stream->readUInt16();
        KonstBlueCount = stream->readUInt16();
        KonstAlphaCount = stream->readUInt16();

        RegisterTrackTableOffset = stream->readUInt32() + BlockOffset;
        KonstTrackTableOffset = stream->readUInt32() + BlockOffset;

        RegisterMaterialInstanceTableOffset = stream->readUInt32() + BlockOffset;
        KonstMaterialInstanceTableOffset = stream->readUInt32() + BlockOffset;

        RegisterMaterialNameTableOffset = stream->readUInt32() + BlockOffset;
        KonstMaterialNameTableOffset = stream->readUInt32() + BlockOffset;

        RegisterRedTableOffset = stream->readUInt32() + BlockOffset;
        RegisterGreenTableOffset = stream->readUInt32() + BlockOffset;
        RegisterBlueTableOffset = stream->readUInt32() + BlockOffset;
        RegisterAlphaTableOffset = stream->readUInt32() + BlockOffset;

        KonstRedTableOffset = stream->readUInt32() + BlockOffset;
        KonstGreenTableOffset = stream->readUInt32() + BlockOffset;
        KonstBlueTableOffset = stream->readUInt32() + BlockOffset;
        KonstAlphaTableOffset = stream->readUInt32() + BlockOffset;
    }
    catch (...) {
        return false;
    }

    return true;
}

bool J3DTexIndexKeyBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream)) {
        return false;
    }

    try {
        LoopMode = stream->readUInt8();
        stream->skip(1);

        Length = stream->readUInt16();
        TrackCount = stream->readUInt16();
        uint16_t unk1 = stream->readUInt16();

        TrackTableOffset = stream->readUInt32() + BlockOffset;
        IndexTableOffset = stream->readUInt32() + BlockOffset;
        MaterialInstanceTableOffset = stream->readUInt32() + BlockOffset;
        MaterialNameTableOffset = stream->readUInt32() + BlockOffset;
    }
    catch (...) {
        return false;
    }

    return true;
}

bool J3DTexMatrixKeyBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream)) {
        return false;
    }

    try {
        LoopMode = stream->readUInt8();
        RotationFraction = stream->readUInt8();

        Length = stream->readUInt16();
        TrackCount = stream->readUInt16();

        ScaleTableCount = stream->readUInt16();
        RotationTableCount = stream->readUInt16();
        TranslationTableCount = stream->readUInt16();

        TrackTableOffset = stream->readUInt32() + BlockOffset;
        MaterialInstanceTableOffset = stream->readUInt32() + BlockOffset;
        MaterialNameTableOffset = stream->readUInt32() + BlockOffset;

        TexGenIndexTableOffset = stream->readUInt32() + BlockOffset;
        TexMatrixOriginTableOffset = stream->readUInt32() + BlockOffset;

        ScaleTableOffset = stream->readUInt32() + BlockOffset;
        RotationTableOffset = stream->readUInt32() + BlockOffset;
        TranslationTableOffset = stream->readUInt32() + BlockOffset;

        stream->readBytesTo(PostTexMatrixData, 0x28);

        MatrixMode = stream->readUInt8();
    }
    catch (...) {
        return false;
    }

    return true;
}

bool J3DJointKeyBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream)) {
        return false;
    }

    try {
        LoopMode = stream->readUInt8();
        RotationFraction = stream->readUInt8();

        Length = stream->readUInt16();
        TrackCount = stream->readUInt16();

        ScaleTableCount = stream->readUInt16();
        RotationTableCount = stream->readUInt16();
        TranslationTableCount = stream->readUInt16();

        TrackTableOffset = stream->readUInt32() + BlockOffset;
        ScaleTableOffset = stream->readUInt32() + BlockOffset;
        RotationTableOffset = stream->readUInt32() + BlockOffset;
        TranslationTableOffset = stream->readUInt32() + BlockOffset;
    }
    catch (...) {
        return false;
    }

    return true;
}

bool J3DJointFullBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream)) {
        return false;
    }

    try {
        LoopMode = stream->readUInt8();
        stream->skip(1);

        Length = stream->readUInt16();
        TrackCount = stream->readUInt16();

        ScaleTableCount = stream->readUInt16();
        RotationTableCount = stream->readUInt16();
        TranslationTableCount = stream->readUInt16();

        TrackTableOffset = stream->readUInt32() + BlockOffset;
        ScaleTableOffset = stream->readUInt32() + BlockOffset;
        RotationTableOffset = stream->readUInt32() + BlockOffset;
        TranslationTableOffset = stream->readUInt32() + BlockOffset;
    }
    catch (...) {
        return false;
    }

    return true;
}

bool J3DVisibilityBlock::Deserialize(bStream::CStream* stream) {
    size_t currentStreamPosition = stream->tell();

    if (!J3DBlock::Deserialize(stream)) {
        return false;
    }

    try {
        LoopMode = stream->readUInt8();
        m0009 = stream->readUInt8();

        Length = stream->readUInt16();
        TrackCount = stream->readUInt16();
        BooleanTableCount = stream->readUInt16();

        TrackTableOffset = stream->readUInt32() + BlockOffset;
        BooleanTableOffset = stream->readUInt32() + BlockOffset;
    }
    catch (...) {
        return false;
    }

    return true;
}
