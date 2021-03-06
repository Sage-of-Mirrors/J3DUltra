#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DVertexShaderGenerator.hpp"
#include "J3D/J3DFragmentShaderGenerator.hpp"
#include "J3D/J3DUniformBufferObject.hpp"

#include <GXGeometryData.hpp>
#include <iostream>
#include <vector>
#include <glad/glad.h>

J3DMaterial::J3DMaterial() : mShaderProgram(-1) {

}

J3DMaterial::~J3DMaterial() {
	if (mShaderProgram != -1)
		glDeleteProgram(mShaderProgram);
}

bool J3DMaterial::GenerateShaders(const int32_t& jointCount) {
	uint32_t vertShader, fragShader;

	if (!J3DVertexShaderGenerator::GenerateVertexShader(this, jointCount, vertShader)) {
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
	glProgramUniform4fv(mShaderProgram, uniformID, 1, &LightBlock.mMatteColor[0][0]);
	uniformID = glGetUniformLocation(mShaderProgram, "uMaterialReg[1]");
	glProgramUniform4fv(mShaderProgram, uniformID, 1, &LightBlock.mMatteColor[1][0]);
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
		case EGXBlendModeControl::SrcColor:
			return GL_SRC_COLOR;
		case EGXBlendModeControl::SrcAlpha:
			return GL_SRC_ALPHA;
		case EGXBlendModeControl::DstAlpha:
			return GL_DST_ALPHA;
		case EGXBlendModeControl::InverseSrcColor:
			return GL_ONE_MINUS_SRC_COLOR;
		case EGXBlendModeControl::InverseSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;
		case EGXBlendModeControl::InverseDstAlpha:
			return GL_ONE_MINUS_DST_ALPHA;
		default:
			return GL_ONE;
	}
}

void J3DMaterial::Render(std::vector<uint32_t>& textureHandles) {
	glUseProgram(mShaderProgram);
	for (int i = 0; i < TevBlock.mTextureIndices.size(); i++)
		glBindTextureUnit(i, textureHandles[TevBlock.mTextureIndices[i]]);

	if (PEBlock.mBlendMode.Type != EGXBlendMode::None)
	{
		glEnable(GL_BLEND);

		switch (PEBlock.mBlendMode.Type)
		{
			case EGXBlendMode::Blend:
				glBlendEquation(GL_FUNC_ADD);
				break;
			case EGXBlendMode::Subtract:
				glBlendEquation(GL_FUNC_SUBTRACT);
				break;
			case EGXBlendMode::Logic:
				glBlendEquation(GL_FUNC_ADD);
				break;
		}

		glBlendFunc(GXBlendModeControlToGLFactor(PEBlock.mBlendMode.SourceFactor), GXBlendModeControlToGLFactor(PEBlock.mBlendMode.DestinationFactor));
	}

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

	J3DUniformBufferObject::SetTevColors(TevBlock.mTevColors);
	J3DUniformBufferObject::SetKonstColors(TevBlock.mTevKonstColors);

	glm::mat4 t[10];
	for (int i = 0; i < 10; i++)
		t[i] = glm::identity<glm::mat4>();

	J3DUniformBufferObject::SetTexMatrices(t);

	if (mShape != nullptr) {
		uint32_t offset, count;
		mShape->GetVertexOffsetAndCount(offset, count);

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (const void*)(offset * sizeof(uint16_t)));
	}

	glUseProgram(0);
	for (int i = 0; i < 8; i++)
		glBindTextureUnit(i, 0);

	glDisable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ZERO);

	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}
