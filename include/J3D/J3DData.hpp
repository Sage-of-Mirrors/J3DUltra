#pragma once

#include <cstdint>

namespace bStream { class CStream; }

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
};

struct J3DDataBase {
  EJ3DVersion J3DVersion;
  EJ3DFormatVersion FormatVersion;
  uint32_t Size;
  uint32_t BlockCount;
  uint8_t Watermark[16];
  
  //J3DBlock Blocks[];
  
  virtual bool Deserialize(bStream::CStream* stream);
};
