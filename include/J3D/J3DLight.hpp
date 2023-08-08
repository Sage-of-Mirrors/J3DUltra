#pragma once

#include <glm/glm.hpp>

struct J3DLight {
	glm::vec4 Position = glm::vec4(0, 0, 0, 1);
	glm::vec4 Direction = glm::vec4(0, 0, 0, 1);
	glm::vec4 Color = glm::vec4(0, 0, 0, 0);
	glm::vec4 AngleAtten = glm::vec4(1, 0, 0, 1);
	glm::vec4 DistAtten = glm::vec4(1, 0, 0, 1);
};
