#pragma once

namespace bStream { class CStream; }
class J3DShapeFactory;

struct J3DVertex {
	uint32_t Position;
	uint32_t Normal;
	uint32_t Color[2];
	uint32_t TexCoord[8];
};

class J3DShape {
	friend J3DShapeFactory;

public:
	J3DShape();
	~J3DShape();

	void Deserialize(bStream::CStream* stream);
};
