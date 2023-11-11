#pragma once

#include <glm/glm.hpp>

#include <memory>

class J3DMaterial;
struct J3DLight;

namespace J3DUniformBufferObject {
	void CreateUBO();
	void DestroyUBO();

	bool LinkMaterialToUBO(const std::shared_ptr<J3DMaterial> material);

	// Updates the UBO's projection and view matrices. Usually used by the environment.
	void SetProjAndViewMatrices(const glm::mat4* proj, const glm::mat4* view);
	// Updates the UBO's model matrix. Usually used by a model instance.
	void SetModelMatrix(const glm::mat4* model);
	// Updates the UBO's TEV color array - assumes an array of 4 elements. Usually used by a model instance.
	void SetTevColors(const glm::vec4* colors);
	// Updates the UBO's konst color array - assumes an array of 4 elements. Usually used by a model instance.
	void SetKonstColors(const glm::vec4* colors);
	// Updates the UBO's light array - assumes an array of 8 elements. Usually used by the environment.
	void SetLights(const J3DLight* lights);
	// Updates the UBO's envelope matrix array - count must be between 1 and 256. Usually used by a model instance.
	void SetEnvelopeMatrices(const glm::mat4* envelopes, const uint32_t count);
	// Updates the UBO's tex matrix array - assumes an array of 10 elements. Usually used by a model instance.
	void SetTexMatrices(const glm::mat4* matrices);
}
