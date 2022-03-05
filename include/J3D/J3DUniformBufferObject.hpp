#pragma once

#include <glm/glm.hpp>

class J3DMaterial;
class J3DLight;

namespace J3DUniformBufferObject {
	void CreateUBO();
	void DestroyUBO();

	bool LinkMaterialToUBO(const J3DMaterial* material);

	void SetProjAndViewMatrices(const glm::mat4* proj, const glm::mat4* view);
	void SetModelMatrix(const glm::mat4* model);
	void SetTevColors(const glm::vec4* colors);
	void SetKonstColors(const glm::vec4* colors);
	void SetLights(const J3DLight* lights);
	void SetEnvelopeMatrices(const glm::mat4* envelopes, const uint32_t count);
	void SetTexMatrices(const glm::mat3x4* matrices);
}
