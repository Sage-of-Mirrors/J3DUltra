#pragma once

#include <cstdint>
#include <vector>
#include "GX/GXEnum.hpp"
#include "glm/glm.hpp"

namespace bStream { class CStream; }
class J3DShapeBlock;
class J3DShape;

struct J3DShapeInitData {
	uint8_t MatrixType;
	uint16_t MatrixNum;
	uint16_t VCDOffset;
	uint16_t MatrixOffset;
	uint16_t DrawOffset;

	float BoundingSphereRadius;
	glm::vec3 BoundingBoxMin;
	glm::vec3 BoundingBoxMax;

	void Deserialize(bStream::CStream* stream);
};

struct J3DShapeMatrixInitData {
	uint16_t ID;
	uint16_t Count;
	uint32_t Start;
};

struct J3DShapeDrawInitData {
	uint32_t Size;
	uint32_t Start;
};

struct J3DVCDData {
	EGXAttribute Attribute;
	EGXComponentType Type;
};

class J3DShapeFactory {
	J3DShapeBlock* mBlock;

public:
	J3DShapeFactory(J3DShapeBlock* srcBlock) { mBlock = srcBlock; }
	~J3DShapeFactory();

	J3DShape* Create(bStream::CStream* stream, uint32_t index);
};
