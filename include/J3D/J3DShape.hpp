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
};

class J3DPacket {
	friend J3DShapeFactory;

	std::vector<EGLAttribute> mEnabledAttributes;
	std::vector<J3DVertex> mVertices;

public:
	J3DPacket();
	~J3DPacket();

	void EnableAttribute(EGLAttribute attr) { mEnabledAttributes.push_back(attr); }
};

class J3DShape {
	friend J3DShapeFactory;

	std::vector<J3DPacket> mPackets;

public:
	J3DShape();
	~J3DShape();

	void Deserialize(bStream::CStream* stream);
};
