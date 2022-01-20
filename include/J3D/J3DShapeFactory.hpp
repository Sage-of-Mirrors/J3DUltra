#pragma once

#include <cstdint>
#include <vector>
#include "GX/GXEnum.hpp"
#include "glm/glm.hpp"

namespace bStream { class CStream; }
class J3DShape;
struct J3DShapeBlock;
struct J3DVertex;

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
	EGXAttributeType Type;
};

class J3DShapeFactory {
	J3DShapeBlock* mBlock;

	uint16_t ConvertPosMtxIndexToDrawIndex(bStream::CStream* stream, const J3DShapeInitData& initData, const uint16_t& packetIndex, const uint16_t& value);
	uint16_t GetUseMatrixValue(bStream::CStream* stream, const J3DShapeInitData& initData, const uint16_t& packetIndex);
	void ReadMatrixInitData(bStream::CStream* stream, J3DShapeMatrixInitData& data, uint32_t index);

	template<typename t>
	std::vector<t> TriangulatePrimitive(EGXPrimitiveType primType, std::vector<t> const& elements) {
		switch (primType) {
		case EGXPrimitiveType::Triangles:
			return elements;
		case EGXPrimitiveType::TriangleStrips:
			return TriangulateTriangleStrip(elements);
		case EGXPrimitiveType::TriangleFan:
			return TriangulateTriangleFan(elements);
		default:
			return std::vector<t>();
		}
	}

	template<typename t>
	std::vector<t> TriangulateTriangleStrip(std::vector<t> const& elements) {
		std::vector<t> triangles;

		for (int i = 2; i < elements.size(); i++) {
			bool isIndexOdd = i % 2 != 0;

			t const& v0 = elements[i - 2];
			t const& v1 = isIndexOdd ? elements[i] : elements[i - 1];
			t const& v2 = isIndexOdd ? elements[i - 1] : elements[i];

			// Reject degenerate triangles (triangles where two or more vertices are the same)
			//if (v0 == v1 || v0 == v2 || v1 == v2)
				//continue;

			triangles.push_back(v0);
			triangles.push_back(v1);
			triangles.push_back(v2);
		}

		return triangles;
	}

	template<typename t>
	std::vector<t> TriangulateTriangleFan(std::vector<t> const& elements) {
		std::vector<t> triangles;

		for (int i = 1; i < elements.size() - 1; i++) {
			t const& v0 = elements[i];
			t const& v1 = elements[i + 1];
			t const& v2 = elements[0];

			// Reject degenerate triangles (triangles where two or more vertices are the same)
			if (v0 == v1 || v0 == v2 || v1 == v2)
				continue;

			triangles.push_back(v0);
			triangles.push_back(v1);
			triangles.push_back(v2);
		}

		return triangles;
	}

public:
	J3DShapeFactory(J3DShapeBlock* srcBlock) { mBlock = srcBlock; }
	~J3DShapeFactory() {}

	J3DShape* Create(bStream::CStream* stream, uint32_t index);
};
