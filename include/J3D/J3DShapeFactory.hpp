#pragma once

#include <GXGeometryEnums.hpp>
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

namespace bStream { class CStream; }
struct J3DShapeBlock;
struct J3DVertex;
class GXShape;

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
	EGXAttributeIndexType Type;
};

class J3DShapeFactory {
	J3DShapeBlock* mBlock;

	uint16_t ConvertPosMtxIndexToDrawIndex(bStream::CStream* stream, const J3DShapeInitData& initData, const uint16_t& packetIndex, const uint16_t& value);
	uint16_t GetUseMatrixValue(bStream::CStream* stream, const J3DShapeInitData& initData, const uint16_t& packetIndex);
	void ReadMatrixInitData(bStream::CStream* stream, J3DShapeMatrixInitData& data, uint32_t index);

public:
	J3DShapeFactory(J3DShapeBlock* srcBlock) { mBlock = srcBlock; }
	~J3DShapeFactory() {}

	GXShape* Create(bStream::CStream* stream, uint32_t index);
};
