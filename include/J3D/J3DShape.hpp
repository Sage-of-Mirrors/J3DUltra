#pragma once

#include <cstdint>
#include <vector>

namespace bStream { class CStream; }
class J3DShapeFactory;

enum class EGLAttribute {
	Position,
	Normal,
	Color0,
	Color1,
	TexCoord0,
	TexCoord1,
	TexCoord2,
	TexCoord3,
	TexCoord4,
	TexCoord5,
	TexCoord6,
	TexCoord7,
	SkinWeight,
	JointID
};

struct J3DVertex {
	uint16_t Position;
	uint16_t Normal;
	uint16_t Color[2];
	uint16_t TexCoord[8];
	uint16_t SkinWeight;
	uint16_t JointID;

	bool operator==(const J3DVertex& other) const {
		return Position == other.Position &&
			Normal == other.Normal &&
			Color[0] == other.Color[0] &&
			Color[1] == other.Color[1] &&
			TexCoord[0] == other.TexCoord[0] &&
			TexCoord[1] == other.TexCoord[1] &&
			TexCoord[2] == other.TexCoord[2] &&
			TexCoord[3] == other.TexCoord[3] &&
			TexCoord[4] == other.TexCoord[4] &&
			TexCoord[5] == other.TexCoord[5] &&
			TexCoord[6] == other.TexCoord[6] &&
			TexCoord[7] == other.TexCoord[7] &&
			SkinWeight == other.SkinWeight &&
			JointID == other.JointID;
	}

	bool operator!=(const J3DVertex& other) const {
		return !operator==(other);
	}
};

class J3DPacket {
	friend J3DShapeFactory;

	std::vector<EGLAttribute> mEnabledAttributes;
	std::vector<J3DVertex> mVertices;

public:
	J3DPacket();
	~J3DPacket();

	void EnableAttributes(std::vector<J3DVCDData> gxAttributes);
};

class J3DShape {
	friend J3DShapeFactory;

	std::vector<J3DPacket> mPackets;

public:
	J3DShape();
	~J3DShape();

	void Deserialize(bStream::CStream* stream);
};
