#pragma once

#include "glm/glm.hpp"

struct J3DTransformInfo {
	glm::vec3 Scale;
	glm::vec3 Rotation;
	glm::vec3 Translation;

	J3DTransformInfo& operator=(J3DTransformInfo const& info) {
		Scale = info.Scale;
		Rotation = info.Rotation;
		Translation = info.Translation;

		return *this;
	}
};

struct J3DTextureSRTInfo {
	glm::vec2 Scale;
	float Rotation;
	glm::vec2 Translation;
};
