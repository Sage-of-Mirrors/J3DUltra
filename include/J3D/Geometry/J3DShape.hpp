#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

namespace bStream { class CStream; }
class J3DShapeFactory;
struct J3DVCDData;

enum class EGLAttribute : uint32_t {
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
	PositionMatrixIdx
};

struct J3DVertexGX {
	uint16_t DrawIndex;
	uint16_t Position;
	uint16_t Normal;
	uint16_t Color[2];
	uint16_t TexCoord[8];

	J3DVertexGX() : DrawIndex(UINT16_MAX), Position(UINT16_MAX), Normal(UINT16_MAX), Color{ UINT16_MAX, UINT16_MAX },
		TexCoord{ UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX } {}

	bool operator==(const J3DVertexGX& other) const {
		return DrawIndex == other.DrawIndex &&
			Position == other.Position &&
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
			TexCoord[7] == other.TexCoord[7];
	}

	bool operator!=(const J3DVertexGX& other) const {
		return !operator==(other);
	}
};

struct J3DVertexGL {
	glm::vec4 Position; // w is envelope index
	glm::vec3 Normal;
	glm::vec4 Color[2];
	glm::vec3 TexCoord[8]; // z is texmatrix index
};

struct J3DPacket {
	// Vertex attribute indices
	std::vector<J3DVertexGX> mVertices;
};

class J3DShape {
	friend J3DShapeFactory;

	std::vector<EGLAttribute> mEnabledAttributes;
	std::vector<J3DPacket> mPackets;

	uint32_t mIBOStart;
	uint32_t mIBOCount;

	glm::vec3 mCenter;

public:
	J3DShape() : mIBOStart(0), mIBOCount(0), mCenter() {}
	~J3DShape() {}

	void EnableAttributes(std::vector<J3DVCDData>& gxAttributes);
	const std::vector<EGLAttribute>& GetEnabledAttributes() const { return mEnabledAttributes; }
	bool HasEnabledAttribute(const EGLAttribute attribute) const;

	void ConcatenatePacketsToIBO(std::vector<J3DVertexGX>& ibo);

	void RenderShape();

	void Deserialize(bStream::CStream* stream);
};
