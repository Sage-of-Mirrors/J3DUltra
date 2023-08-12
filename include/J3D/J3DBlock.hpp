#pragma once

#include "J3DData.hpp"
#include "GX/GXStruct.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include <GXGeometryEnums.hpp>
#include <GXVertexData.hpp>
#include <cstdint>
#include <vector>

namespace bStream { class CStream; }
class GXAttributeData;

enum class EJ3DBlockType : uint32_t {
  ANF1 = 0x414E4631, // Joint full animation
  ANK1 = 0x414E4B31, // Joint keyframe animation
  
  CLF1 = 0x434C4631, // Cluster full animation
  CLK1 = 0x434C4B31, // Cluster keyframe animation
  
  DRW1 = 0x44525731, // Model geometry -> joint map
  EVP1 = 0x45565031, // Model joint envelopes
  INF1 = 0x494E4631, // Model scenegraph
  JNT1 = 0x4A4E5431, // Model joints
  
  MAT1 = 0x4D415431, // Model materials, version 1
  MAT2 = 0x4D415432, // Model materials, version 2
  MAT3 = 0x4D415433, // Model materials, version 3
  
  MDL3 = 0x4D444C33, // Precompiled model materials, version 3
  
  PAF1 = 0x50414631, // Color full animation
  PAK1 = 0x50414B31, // Color keyframe animation
  
  SHP1 = 0x53485031, // Model geometry
  TEX1 = 0x54455831, // Textures
  TRK1 = 0x54524B31, // TEV register keyframe animation
  TTK1 = 0x54544B31, // Texture transform keyframe animation
  TPT1 = 0x54505431, // Texture flipbook animation
  
  VAF1 = 0x56414631, // Vertex alpha full animation
  
  VCF1 = 0x56434631, // Vertex color full animation
  VCK1 = 0x56434B31, // Vertex color keyframe animation
  
  VTX1 = 0x56545831  // Model vertex data
};

enum class EJ3DHierarchyType : uint16_t {
  Exit,
  Begin,
  End,
  
  Joint = 16,
  Material,
  Shape
};

// Represents a section of a J3D file (INF1, JNT1, ANK1, etc)
struct J3DBlock {
  uint32_t BlockOffset;
  EJ3DBlockType BlockType;
  uint32_t BlockSize;

  virtual bool Deserialize(bStream::CStream* stream);
};

struct J3DModelHierarchy {
  EJ3DHierarchyType Type;
  uint16_t Index;
};

struct J3DModelInfoBlock : public J3DBlock {
  uint16_t Flags;
  uint32_t MatrixGroupCount;
  uint32_t VertexPositionCount;

  uint32_t HierarchyOffset;

  virtual bool Deserialize(bStream::CStream* stream) override;
};

struct J3DVertexBlock : public J3DBlock {
  uint32_t AttributeTableOffset; // 0x0008
  uint32_t PositionTableOffset; // 0x000C
  uint32_t NormalTableOffset; // 0x0010
  uint32_t NBTTableOffset; // 0x0014
  uint32_t ColorTablesOffset[2]; // 0x0018
  uint32_t TexCoordTablesOffset[8]; // 0x0020

  virtual bool Deserialize(bStream::CStream* stream) override;

  void LoadAttributeData(GXAttributeData* vertexData, bStream::CStream* stream, GXVertexAttributeFormat& curAttribute, GXVertexAttributeFormat& nextAttribute);

private:
	uint32_t GetAttributeElementCount(GXVertexAttributeFormat& curAttribute);
	uint32_t CalculateAttributeCount(GXVertexAttributeFormat& curAttribute, GXVertexAttributeFormat& nextAttribute);
};

struct J3DEnvelopeBlock : public J3DBlock {
  uint16_t Count; // 0x0008

  uint32_t JointIndexTableOffset; // 0x000C
  uint32_t EnvelopeIndexTableOffset; // 0x0010
  uint32_t WeightTableOffset; // 0x0014
  uint32_t MatrixTableOffset; // 0x0018

  virtual bool Deserialize(bStream::CStream* stream) override;
};

struct J3DDrawBlock : public J3DBlock {
  uint16_t Count; // 0x0008
  
  uint32_t DrawTableOffset; // 0x000C
  uint32_t IndexTableOffset; // 0x0010

  virtual bool Deserialize(bStream::CStream* stream) override;
};

struct J3DJointInitData {
  uint16_t MatrixType;
  uint8_t AttachFlag;
  //J3DTransformInfo Transform;
  float BoundingSphereRadius;
  glm::vec3 BoundingBoxMin;
  glm::vec3 BoundingBoxMax;
};

struct J3DJointBlock : public J3DBlock {
  uint16_t Count; // 0x0008

  uint32_t InitDataTableOffset; // 0x000C
  uint32_t IndexTableOffset; // 0x0010
  uint32_t NameTableOffset; // 0x0014

  virtual bool Deserialize(bStream::CStream* stream) override;
};

struct J3DShapeBlock : public J3DBlock {
  uint16_t Count; // 0x0008

  uint32_t InitDataTableOffset; // 0x000C
  uint32_t IndexTableOffset; // 0x0010
  uint32_t NameTableOffset; // 0x0014, not used?
  uint32_t VertexDescriptorTableOffset; // 0x0018
  uint32_t MatrixTableOffset; // 0x001C
  uint32_t DrawTableOffset; // 0x0020
  uint32_t MatrixInitTableOffset; // 0x0024
  uint32_t DrawInitDataTableOffset; // 0x0028
  
  virtual bool Deserialize(bStream::CStream* stream) override;
};

struct J3DMaterialBlockV2 : public J3DBlock {
	uint16_t Count;

	uint32_t InitDataTableOffset;
	uint32_t InstanceTableOffset;
	uint32_t NameTableOffset;
	uint32_t CullModeTableOffset;
	uint32_t MaterialColorTableOffset;
	uint32_t ColorChannelCountTableOffset;
	uint32_t ColorChannelTableOffset;
	uint32_t TexGenCountTableOffset;
	uint32_t TexCoordTableOffset;
	uint32_t TexCoord2TableOffset;
	uint32_t TexMatrixTableOffset;
	uint32_t PostTexMatrixTableOffset;
	uint32_t TextureIndexTableOffset;
	uint32_t TevOrderTableOffset;
	uint32_t TevColorTableOffset;
	uint32_t TevKColorTableOffset;
	uint32_t TevStageCountTableOffset;
	uint32_t TevStageTableOffset;
	uint32_t TevSwapModeTableOffset;
	uint32_t TevSwapTableOffset;
	uint32_t FogTableOffset;
	uint32_t AlphaCompareTableOffset;
	uint32_t BlendInfoTableOffset;
	uint32_t ZModeTableOffset;
	uint32_t ZCompLocTableOffset;
	uint32_t DitherTableOffset;
	uint32_t NBTScaleTableOffset;

	virtual bool Deserialize(bStream::CStream* stream) override;
};

struct J3DMaterialBlockV3 : public J3DBlock {
	uint16_t Count;

	uint32_t InitDataTableOffset;
	uint32_t InstanceTableOffset;
	uint32_t NameTableOffset;
	uint32_t IndirectInitDataTableOffset;
	uint32_t CullModeTableOffset;
	uint32_t MaterialColorTableOffset;
	uint32_t ColorChannelCountTableOffset;
	uint32_t ColorChannelTableOffset;
	uint32_t AmbientColorTableOffset;
	uint32_t LightTableOffset;
	uint32_t TexGenCountTableOffset;
	uint32_t TexCoordTableOffset;
	uint32_t TexCoord2TableOffset;
	uint32_t TexMatrixTableOffset;
	uint32_t PostTexMatrixTableOffset;
	uint32_t TextureIndexTableOffset;
	uint32_t TevOrderTableOffset;
	uint32_t TevColorTableOffset;
	uint32_t TevKColorTableOffset;
	uint32_t TevStageCountTableOffset;
	uint32_t TevStageTableOffset;
	uint32_t TevSwapModeTableOffset;
	uint32_t TevSwapTableOffset;
	uint32_t FogTableOffset;
	uint32_t AlphaCompareTableOffset;
	uint32_t BlendInfoTableOffset;
	uint32_t ZModeTableOffset;
	uint32_t ZCompLocTableOffset;
	uint32_t DitherTableOffset;
	uint32_t NBTScaleTableOffset;

	virtual bool Deserialize(bStream::CStream* stream) override;
};

struct J3DTextureBlock : public J3DBlock {
	uint16_t Count;

	uint32_t TexTableOffset;
	uint32_t NameTableOffset;

	virtual bool Deserialize(bStream::CStream* stream) override;
};

struct J3DRegisterColorKeyBlock : public J3DBlock {
	uint8_t LoopMode;

	uint16_t Length;
	uint16_t RegisterTrackCount;
	uint16_t KonstTrackCount;

	uint16_t RegisterRedCount;
	uint16_t RegisterGreenCount;
	uint16_t RegisterBlueCount;
	uint16_t RegisterAlphaCount;

	uint16_t KonstRedCount;
	uint16_t KonstGreenCount;
	uint16_t KonstBlueCount;
	uint16_t KonstAlphaCount;

	uint32_t RegisterTrackTableOffset;
	uint32_t KonstTrackTableOffset;

	uint32_t RegisterMaterialInstanceTableOffset;
	uint32_t KonstMaterialInstanceTableOffset;

	uint32_t RegisterMaterialNameTableOffset;
	uint32_t KonstMaterialNameTableOffset;

	uint32_t RegisterRedTableOffset;
	uint32_t RegisterGreenTableOffset;
	uint32_t RegisterBlueTableOffset;
	uint32_t RegisterAlphaTableOffset;

	uint32_t KonstRedTableOffset;
	uint32_t KonstGreenTableOffset;
	uint32_t KonstBlueTableOffset;
	uint32_t KonstAlphaTableOffset;

	virtual bool Deserialize(bStream::CStream* stream) override;
};

struct J3DTexIndexKeyBlock : public J3DBlock {
	uint8_t LoopMode;

	uint16_t Length;
	uint16_t TrackCount;

	uint32_t TrackTableOffset;
	uint32_t IndexTableOffset;
	uint32_t MaterialInstanceTableOffset;
	uint32_t MaterialNameTableOffset;

	virtual bool Deserialize(bStream::CStream* stream) override;
};