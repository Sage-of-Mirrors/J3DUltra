#pragma once

#include <vector>
#include "glm/mat4x4.hpp"

struct J3DEnvelope {
	std::vector<uint16_t> JointIndices;
	std::vector<float> Weights;
};
