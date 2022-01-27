#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

namespace bStream { class CStream; }

struct J3DTransformInfo {
	glm::vec3 Scale;
	glm::quat Rotation;
	glm::vec3 Translation;

	J3DTransformInfo& operator=(J3DTransformInfo const& info) {
		Scale = info.Scale;
		Rotation = info.Rotation;
		Translation = info.Translation;

		return *this;
	}

	void Deserialize(bStream::CStream* stream);
	glm::mat4 ToMat4();
};

struct J3DTextureSRTInfo {
	glm::vec2 Scale;
	float Rotation;
	glm::vec2 Translation;

	void Deserialize(bStream::CStream* stream);
	glm::mat4 ToMat4();
};
