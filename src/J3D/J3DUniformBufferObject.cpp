#include "J3D/J3DUniformBufferObject.hpp"
#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DLight.hpp"

#include <glad/glad.h>
#include <cstdint>

namespace J3DUniformBufferObject {
	namespace {
		constexpr uint32_t LIGHTS_MAX = 8;
		constexpr uint32_t COLOR_MAX = 4;
		constexpr uint32_t ENVELOPE_MAT_MAX = 256;
		constexpr uint32_t TEX_MAT_MAX = 10;
		constexpr char* UBO_NAME = "Matrices";

		struct J3DUniformBufferObject {
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewMatrix;
			glm::mat4 ModelMatrix;

			glm::vec4 TevColor[COLOR_MAX];
			glm::vec4 KonstColor[COLOR_MAX];

			J3DLight Lights[LIGHTS_MAX];
			glm::mat4 EnvelopeMatrices[ENVELOPE_MAT_MAX];
			glm::mat3x4 TexMatrices[TEX_MAT_MAX];
		};

		uint32_t mUBO = 0;
	}
}

void J3DUniformBufferObject::CreateUBO() {
	glCreateBuffers(1, &mUBO);

	glNamedBufferStorage(mUBO, sizeof(J3DUniformBufferObject), nullptr, GL_DYNAMIC_STORAGE_BIT);
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

	glNamedBufferSubData(mUBO, offsetof(J3DUniformBufferObject, ProjectionMatrix), sizeof(glm::mat4), proj);
	glNamedBufferSubData(mUBO, offsetof(J3DUniformBufferObject, ViewMatrix), sizeof(glm::mat4), view);
}

void J3DUniformBufferObject::SetModelMatrix(const glm::mat4* model) {
	if (mUBO == 0)
		return;

	glNamedBufferSubData(mUBO, offsetof(J3DUniformBufferObject, ModelMatrix), sizeof(glm::mat4), model);
}

void J3DUniformBufferObject::SetTevColors(const glm::vec4* colors) {
	if (mUBO == 0)
		return;

	glNamedBufferSubData(mUBO, offsetof(J3DUniformBufferObject, TevColor), sizeof(glm::vec4) * COLOR_MAX, colors);
}

void J3DUniformBufferObject::SetKonstColors(const glm::vec4* colors) {
	if (mUBO == 0)
		return;

	glNamedBufferSubData(mUBO, offsetof(J3DUniformBufferObject, KonstColor), sizeof(glm::vec4) * COLOR_MAX, colors);
}

void J3DUniformBufferObject::SetLights(const J3DLight* lights) {
	if (mUBO == 0)
		return;

	glNamedBufferSubData(mUBO, offsetof(J3DUniformBufferObject, Lights), sizeof(J3DLight) * LIGHTS_MAX, lights);
}

void J3DUniformBufferObject::SetEnvelopeMatrices(const glm::mat4* envelopes, const uint32_t count) {
	if (mUBO == 0 || count > ENVELOPE_MAT_MAX)
		return;

	glNamedBufferSubData(mUBO, offsetof(J3DUniformBufferObject, EnvelopeMatrices), sizeof(glm::mat4) * count, envelopes);
}

void J3DUniformBufferObject::SetTexMatrices(const glm::mat3x4* envelopes) {
	if (mUBO == 0)
		return;

	glNamedBufferSubData(mUBO, offsetof(J3DUniformBufferObject, TexMatrices), sizeof(glm::mat3x4) * TEX_MAT_MAX, envelopes);
}
