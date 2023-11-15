#include "J3D/Material/J3DUniformBufferObject.hpp"
#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Rendering/J3DLight.hpp"

#include <glad/glad.h>

#include <cstdint>
#include <algorithm>

namespace J3DUniformBufferObject {
	namespace {
		constexpr uint32_t LIGHTS_MAX = 8;
		constexpr uint32_t COLORS_MAX = 4;
		constexpr uint32_t ENVELOPE_MATS_MAX = 512;
		constexpr uint32_t TEX_MATS_MAX = 10;
		constexpr char* UBO_NAME = "uSharedData";

		struct J3DUniformBufferObject {
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewMatrix;
			glm::mat4 ModelMatrix;

			glm::vec4 TevColor[COLORS_MAX];
			glm::vec4 KonstColor[COLORS_MAX];

			J3DLight Lights[LIGHTS_MAX];
			glm::mat4 Envelopes[ENVELOPE_MATS_MAX];
			glm::mat4 TexMatrices[TEX_MATS_MAX];

			uint32_t BillboardType;

			J3DUniformBufferObject() { ClearUBO(); }
		};

		static J3DUniformBufferObject mUBO;
		uint32_t mUBOID = 0;
	}
}

void J3DUniformBufferObject::CreateUBO() {
	glCreateBuffers(1, &mUBOID);

	glNamedBufferStorage(mUBOID, sizeof(J3DUniformBufferObject), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, mUBOID, 0, sizeof(J3DUniformBufferObject));
}

void J3DUniformBufferObject::DestroyUBO() {
	if (mUBOID == 0)
		return;

	glDeleteBuffers(1, &mUBOID);
	mUBOID = 0;
}

void J3DUniformBufferObject::SubmitUBO() {
	if (mUBOID == 0)
		return;

	glNamedBufferSubData(mUBOID, NULL, sizeof(J3DUniformBufferObject), &mUBO);
}

void J3DUniformBufferObject::ClearUBO() {
	mUBO.ProjectionMatrix = glm::identity<glm::mat4>();
	mUBO.ViewMatrix = glm::identity<glm::mat4>();
	mUBO.ModelMatrix = glm::identity<glm::mat4>();

	std::fill_n(mUBO.TevColor, COLORS_MAX, glm::one<glm::vec4>());
	std::fill_n(mUBO.KonstColor, COLORS_MAX, glm::one<glm::vec4>());

	std::fill_n(mUBO.Lights, LIGHTS_MAX, DEFAULT_LIGHT);

	std::fill_n(mUBO.Envelopes, ENVELOPE_MATS_MAX, glm::identity<glm::mat4>());
	std::fill_n(mUBO.TexMatrices, TEX_MATS_MAX, glm::identity<glm::mat4>());

	mUBO.BillboardType = 0;
}

bool J3DUniformBufferObject::LinkMaterialToUBO(const std::shared_ptr<J3DMaterial> material) {
	if (mUBOID == 0)
		CreateUBO();

	int32_t shaderProgram = material->GetShaderProgram();

	uint32_t uniformIndex = glGetUniformBlockIndex(shaderProgram, UBO_NAME);
	glUniformBlockBinding(shaderProgram, uniformIndex, 0);

	return true;
}

void J3DUniformBufferObject::SetProjAndViewMatrices(const glm::mat4& proj, const glm::mat4& view) {
	mUBO.ProjectionMatrix = proj;
	mUBO.ViewMatrix = view;
}

void J3DUniformBufferObject::SetModelMatrix(const glm::mat4& model) {
	mUBO.ModelMatrix = model;
}

void J3DUniformBufferObject::SetTevColors(const glm::vec4* colors) {
	std::memcpy(mUBO.TevColor, colors, sizeof(glm::vec4) * COLORS_MAX);
}

void J3DUniformBufferObject::SetKonstColors(const glm::vec4* colors) {
	std::memcpy(mUBO.KonstColor, colors, sizeof(glm::vec4) * COLORS_MAX);
}

void J3DUniformBufferObject::SetLights(const J3DLight* lights) {
	std::memcpy(mUBO.Lights, lights, sizeof(J3DLight) * LIGHTS_MAX);
}

void J3DUniformBufferObject::SetEnvelopeMatrices(const glm::mat4* envelopes, const uint32_t count) {
	if (count == 0 || count > ENVELOPE_MATS_MAX)
		return;

	std::memcpy(mUBO.Envelopes, envelopes, sizeof(glm::mat4) * count);
}

void J3DUniformBufferObject::SetTexMatrices(const glm::mat4* texMatrices) {
	std::memcpy(mUBO.TexMatrices, texMatrices, sizeof(glm::mat4) * TEX_MATS_MAX);
}

void J3DUniformBufferObject::SetBillboardType(const uint32_t& type) {
	mUBO.BillboardType = type;
}
