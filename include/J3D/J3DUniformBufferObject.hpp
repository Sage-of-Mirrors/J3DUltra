#pragma once

#include <glm/glm.hpp>

class J3DMaterial;

namespace J3DUniformBufferObject {
	void CreateUBO();
	void DestroyUBO();

	bool LinkMaterialToUBO(const J3DMaterial* material);

	void SetProjAndViewMatrices(const glm::mat4* proj, const glm::mat4* view);
	void SetModelMatrix(const glm::mat4* model);
	void SetEnvelopeMatrices(const glm::mat4* envelopes);
}
