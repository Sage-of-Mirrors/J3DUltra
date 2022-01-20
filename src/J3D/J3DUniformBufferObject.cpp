#include "J3D/J3DUniformBufferObject.hpp"
#include "J3D/J3DMaterial.hpp"

#include <glad/glad.h>
#include <cstdint>

namespace J3DUniformBufferObject {
	namespace {
		constexpr uint32_t ENVELOPE_MAT_MAX = 256;
		constexpr char* UBO_NAME = "Matrices";

		struct J3DUniformBufferObject {
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewMatrix;
			glm::mat4 ModelMatrix;

			glm::mat4 EnvelopeMatrices[ENVELOPE_MAT_MAX];
		};

		uint32_t mUBO = 0;
	}
}

void J3DUniformBufferObject::CreateUBO() {
	glGenBuffers(1, &mUBO);

	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(J3DUniformBufferObject), nullptr, GL_STATIC_DRAW);
	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, mUBO, 0, sizeof(J3DUniformBufferObject));
}

void J3DUniformBufferObject::DestroyUBO() {
	if (mUBO == 0)
		return;

	glDeleteBuffers(1, &mUBO);
	mUBO = 0;
}

bool J3DUniformBufferObject::LinkMaterialToUBO(const J3DMaterial* material) {
	if (mUBO == 0)
		CreateUBO();

	int32_t shaderProgram = material->GetShaderProgram();

	uint32_t uniformIndex = glGetUniformBlockIndex(shaderProgram, UBO_NAME);
	glUniformBlockBinding(shaderProgram, uniformIndex, 0);

	return true;
}

void J3DUniformBufferObject::SetProjAndViewMatrices(const glm::mat4* proj, const glm::mat4* view) {
	if (mUBO == 0)
		return;

	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(J3DUniformBufferObject, ProjectionMatrix), sizeof(glm::mat4), proj);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(J3DUniformBufferObject, ViewMatrix), sizeof(glm::mat4), view);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void J3DUniformBufferObject::SetModelMatrix(const glm::mat4* model) {
	if (mUBO == 0)
		return;

	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(J3DUniformBufferObject, ModelMatrix), sizeof(glm::mat4), model);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void J3DUniformBufferObject::SetEnvelopeMatrices(const glm::mat4* envelopes) {
	if (mUBO == 0)
		return;

	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(J3DUniformBufferObject, EnvelopeMatrices), sizeof(glm::mat4) * ENVELOPE_MAT_MAX, envelopes);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void J3DUniformBufferObject::BindUBO() {
	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
}

void J3DUniformBufferObject::UnbindUBO() {
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
