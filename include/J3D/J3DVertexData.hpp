#pragma once

#include <cstdint>
#include <vector>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

struct J3DVertexData {
	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec4> Colors[2];
	std::vector<glm::vec2> TexCoords[8];

	uint32_t GetPositionCount() { return Positions.size(); }
	uint32_t GetNormalCount() { return Normals.size(); }
	uint32_t GetColorCount(uint32_t index) { return index >= 0 && index < 2 ? Colors[index].size() : 0; }
	uint32_t GetTexCoordCount(uint32_t index) { return index >= 0 && index < 8 ? TexCoords[index].size() : 0; }

	bool HasPositionData() { return Positions.size() != 0; }
	bool HasNormalData()  { return Normals.size() != 0; }
	bool HasColorData(uint32_t index) { return index >= 0 && index < 2 && Colors[index].size() != 0; }
	bool HasTexCoordData(uint32_t index) { return index >= 0 && index < 8 && TexCoords[index].size() != 0; }
};
