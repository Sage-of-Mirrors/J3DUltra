#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Material/J3DVertexShaderGenerator.hpp"
#include "J3D/Material/J3DFragmentShaderGenerator.hpp"
#include "J3D/Material/J3DUniformBufferObject.hpp"
#include "J3D/Texture/J3DTexture.hpp"

#include <GXGeometryData.hpp>
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <atomic>

std::atomic<uint16_t> J3DMaterial::sMaterialIdSrc = 1;

J3DMaterial::J3DMaterial() : mShaderProgram(-1), AreRegisterColorsAnimating(false), AreTexIndicesAnimating(false),
	mShape(std::weak_ptr<GXShape>()), bSelected(false), mMaterialId(sMaterialIdSrc++) {
	TevBlock = std::make_shared<J3DTevBlock>();
}

J3DMaterial::~J3DMaterial() {
	if (mShaderProgram != -1) {
		glDeleteProgram(mShaderProgram);
	}
}

bool J3DMaterial::GenerateShaders() {
	uint32_t vertShader, fragShader;

	if (mShaderProgram != -1) {
		glDeleteProgram(mShaderProgram);
	}

	if (!J3DVertexShaderGenerator::GenerateVertexShader(this, vertShader)) {
		std::cout << "Error in vertex shader generator!" << std::endl;
		return false;
	}

	if (!J3DFragmentShaderGenerator::GenerateFragmentShader(this, fragShader)) {
		std::cout << "Error in fragment shader generator!" << std::endl;

		glDeleteShader(vertShader);
		return false;
	}

	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, vertShader);
	glAttachShader(mShaderProgram, fragShader);

	glLinkProgram(mShaderProgram);

	int32_t isLinked = 0;
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &isLinked);
	if (!isLinked) {
		std::cout << "Shader program for material " << Name << " failed to link. Error is as follows:" << std::endl;

		// Get the length of the program's log
		int32_t logLength = 0;
		glGetProgramiv(mShaderProgram, GL_INFO_LOG_LENGTH, &logLength);

		// Copy the log into a vector
		std::vector<char> infoLog(logLength);
		glGetProgramInfoLog(mShaderProgram, logLength, &logLength, &infoLog[0]);

		// Turn the vector into a string and output to console
		std::cout << std::string(infoLog.begin(), infoLog.end()) << std::endl;

		// Cleanup program and shaders
		glDeleteProgram(mShaderProgram);
		mShaderProgram = -1;

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		return false;
	}

	for (int i = 0; i < 8; i++) {
		std::string name = "Texture[" + std::to_string(i) + "]";
		uint32_t uniformID = glGetUniformLocation(mShaderProgram, name.c_str());

		glProgramUniform1i(mShaderProgram, uniformID, i);
	}

	glm::vec4 test(0, 0, 0, 1.0);

	uint32_t uniformID = glGetUniformLocation(mShaderProgram, "uMaterialReg[0]");
	glProgramUniform4fv(mShaderProgram, uniformID, 1, &LightBlock.mMaterialColor[0][0]);
	uniformID = glGetUniformLocation(mShaderProgram, "uMaterialReg[1]");
	glProgramUniform4fv(mShaderProgram, uniformID, 1, &LightBlock.mMaterialColor[1][0]);
	uniformID = glGetUniformLocation(mShaderProgram, "uAmbientReg[0]");
	glProgramUniform4fv(mShaderProgram, uniformID, 1, &LightBlock.mAmbientColor[0][0]);
	uniformID = glGetUniformLocation(mShaderProgram, "uAmbientReg[1]");
	glProgramUniform4fv(mShaderProgram, uniformID, 1, &LightBlock.mAmbientColor[1][0]);

	// Program linked successfully, detach and delete the shaders because they're not needed now
	glDetachShader(mShaderProgram, vertShader);
	glDetachShader(mShaderProgram, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return true;
}

int GXBlendModeControlToGLFactor(EGXBlendModeControl Control)
{
	switch (Control)
	{
		case EGXBlendModeControl::Zero:
			return GL_ZERO;
		case EGXBlendModeControl::One:
			return GL_ONE;
		case EGXBlendModeControl::SrcAlpha:
			return GL_SRC_ALPHA;
		case EGXBlendModeControl::InverseSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;
		case EGXBlendModeControl::DstAlpha:
			return GL_DST_ALPHA;
		case EGXBlendModeControl::InverseDstAlpha:
			return GL_ONE_MINUS_DST_ALPHA;
		default:
			return GL_ZERO;
	}
}

int GXSrcBlendModeControlToGLFactor(EGXBlendModeControl control) {
	switch (control) {
		case EGXBlendModeControl::SrcColor:
			return GL_DST_COLOR;
		case EGXBlendModeControl::InverseSrcColor:
			return GL_ONE_MINUS_DST_COLOR;
		default:
			return GXBlendModeControlToGLFactor(control);
	}
}

int GXDstBlendModeControlToGLFactor(EGXBlendModeControl control) {
	switch (control) {
		case EGXBlendModeControl::SrcColor:
			return GL_SRC_COLOR;
		case EGXBlendModeControl::InverseSrcColor:
			return GL_ONE_MINUS_SRC_COLOR;
		default:
			return GXBlendModeControlToGLFactor(control);
	}
}

void J3DMaterial::BindJ3DShader(const std::vector<std::shared_ptr<J3DTexture>>& textures) {
	glUseProgram(mShaderProgram);
	for (int i = 0; i < TevBlock->mTextureIndices.size(); i++)
	{
		uint16_t texIndex = TevBlock->mTextureIndices[i];
		if (AreTexIndicesAnimating) {
			texIndex = AnimationTexIndices[i];
		}

		glBindTextureUnit(i, textures[texIndex]->TexHandle);
	}

	if (PEBlock.mBlendMode.Type != EGXBlendMode::None)
	{
		glEnable(GL_BLEND);

		switch (PEBlock.mBlendMode.Type)
		{
			case EGXBlendMode::Blend:
				glBlendEquation(GL_FUNC_ADD);
				glBlendFunc(GXSrcBlendModeControlToGLFactor(PEBlock.mBlendMode.SourceFactor), GXDstBlendModeControlToGLFactor(PEBlock.mBlendMode.DestinationFactor));
				break;
			case EGXBlendMode::Subtract:
				glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
				glBlendFunc(GL_ONE, GL_ONE);
				break;
			case EGXBlendMode::Logic:
				glBlendEquation(GL_FUNC_ADD);
				break;
		}
	}
	else {
		glDisable(GL_BLEND);
	}

	J3DUniformBufferObject::SetTexMatrices(TexMatrices);

	if (IndirectBlock && IndirectBlock->mEnabled) {
		for (uint32_t i = 0; i < 3; i++) {
			glm::mat4 indTexMat = glm::mat4(IndirectBlock->mIndirectTexMatrices[i]->TexMatrix);
			J3DUniformBufferObject::SetIndTexMatrix(&indTexMat, i);
		}
	}

	if (AreRegisterColorsAnimating) {
		J3DUniformBufferObject::SetTevColors(AnimationRegisterColors);
		J3DUniformBufferObject::SetKonstColors(AnimationKonstColors);
	}
	else {
		J3DUniformBufferObject::SetTevColors(TevBlock->mTevColors);
		J3DUniformBufferObject::SetKonstColors(TevBlock->mTevKonstColors);
	}

	if (bSelected) {
		J3DUniformBufferObject::SetHighlightColor(glm::vec4(0.5f, 0.5f, 0.25f, 0.0f));
	}
	else {
		J3DUniformBufferObject::SetHighlightColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	}
}

void J3DMaterial::ConfigureGLState() {
	if (LightBlock.mCullMode != EGXCullMode::None)
	{
		glEnable(GL_CULL_FACE);

		switch (LightBlock.mCullMode)
		{
			case (EGXCullMode::All):
				glCullFace(GL_FRONT_AND_BACK);
				break;
			case (EGXCullMode::Front):
				glCullFace(GL_BACK);
				break;
			case (EGXCullMode::Back):
				glCullFace(GL_FRONT);
				break;
		}
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	if (PEBlock.mZMode.Enable == true)
	{
		glEnable(GL_DEPTH_TEST);

		switch (PEBlock.mZMode.Function)
		{
			case EGXCompareType::Always:
				glDepthFunc(GL_ALWAYS);
				break;
			case EGXCompareType::Never:
				glDepthFunc(GL_NEVER);
				break;
			case EGXCompareType::Equal:
				glDepthFunc(GL_EQUAL);
				break;
			case EGXCompareType::GEqual:
				glDepthFunc(GL_GEQUAL);
				break;
			case EGXCompareType::Greater:
				glDepthFunc(GL_GREATER);
				break;
			case EGXCompareType::LEqual:
				glDepthFunc(GL_LEQUAL);
				break;
			case EGXCompareType::Less:
				glDepthFunc(GL_LESS);
				break;
			case EGXCompareType::NEqual:
				glDepthFunc(GL_NOTEQUAL);
				break;
		}

		glDepthMask(PEBlock.mZMode.UpdateEnable ? GL_TRUE : GL_FALSE);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
}

void J3DMaterial::Render(const std::vector<std::shared_ptr<J3DTexture>>& textures, uint32_t shaderOverride) {
	if (shaderOverride == 0) {
		BindJ3DShader(textures);
	}
	else {
		glUseProgram(shaderOverride);
		J3DUniformBufferObject::SetMaterialId(mMaterialId);
	}

	if (mShape.expired()) {
		return;
	}

	std::shared_ptr<GXShape> lockedShape = mShape.lock();

	if (lockedShape->GetVisible()) {
		ConfigureGLState();

		J3DUniformBufferObject::SetBillboardType(*lockedShape->GetUserData<uint32_t>());
		J3DUniformBufferObject::SubmitUBO();

		uint32_t offset, count;
		lockedShape->GetVertexOffsetAndCount(offset, count);

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (const void*)(offset * sizeof(uint32_t)));
	}
}

void J3DMaterial::CalculateTexMatrices(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projMatrix) {
	for (int i = 0; i < TexGenBlock.mTexMatrix.size(); i++) {
		TexGenBlock.mTexMatrix[i]->CalculateMatrix(modelMatrix, viewMatrix, projMatrix);
		TexMatrices[i] = TexGenBlock.mTexMatrix[i]->CalculatedMatrix;
	}
}
