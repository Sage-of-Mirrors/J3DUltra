#pragma once

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

enum class EJ3DVersion : uint32_t {
  J3D1 = 0x4A334431, // Version 1, used in most animation files
  J3D2 = 0x4A334432  // Version 2, used in model files
};

enum class EJ3DFormatVersion : uint32_t {
  BCA1 = 0x62636131, // Joint animation with transforms for every frame
  BCK1 = 0x62636B31, // Joint animation with keyed transforms
  
  BDL4 = 0x62646C34, // Model with pre-compiled material display lists
  
  BLA1 = 0x626C6131, // Cluster animation with transforms for every frame
  BLK1 = 0x626C6B31, // Cluster animation with keyed transforms
  
  BMD1 = 0x626D6431, // Model, version 1
  BMD2 = 0x626D6432, // Model, version 2
  BMD3 = 0x626D6433, // Model, version 3
  
  BMT3 = 0x626D7433, // External material data, version 3
  
  BPK1 = 0x62706B31, // Color animation with keyed colors
  BRK1 = 0x62726B31, // TEV register animation with keyed colors
  BTK1 = 0x62746B31, // Texture animation with keyed transforms
  BTP1 = 0x62747031, // Texture flipbook animation
  
  BXA1 = 0x62786131, // Vertex color animation with colors for every frame
  BXK1 = 0x62786B31  // Vertex color animation with keyed colors
}

// Represents a section of a J3D file (INF1, JNT1, ANK1, etc)
struct J3DBlock {
  EJ3DBlockType BlockType;
  uint32_t BlockSize;
};

struct J3DDataBase {
  EJ3DVersion J3DVersion;
  EJ3DFormatVersion FormatVersion;
  uint32_t Size;
  uint32_t BlockCount;
  uint8_t Watermark[16];
  
  J3DBlock Blocks[];
};
