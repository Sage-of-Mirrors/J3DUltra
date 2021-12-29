#pragma once

#include <cstdint>
#include <vector>
#include "J3DData.hpp"
#include "GX/GXStruct.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace bStream { class CStream; }
struct J3DVertexData;

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

  void LoadAttributeData(J3DVertexData* vertexData, bStream::CStream* stream, GXVertexAttributeList& curAttribute, GXVertexAttributeList& nextAttribute);

private:
	uint32_t GetAttributeElementCount(GXVertexAttributeList& curAttribute);
	uint32_t CalculateAttributeCount(GXVertexAttributeList& curAttribute, GXVertexAttributeList& nextAttribute);
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
