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

void J3DMaterial::Render(std::vector<uint32_t>& textureHandles) {
	glUseProgram(mShaderProgram);
	for (int i = 0; i < TevBlock.mTextureIndices.size(); i++)
		glBindTextureUnit(i, textureHandles[TevBlock.mTextureIndices[i]]);

	J3DUniformBufferObject::SetTevColors(TevBlock.mTevColors);
	J3DUniformBufferObject::SetKonstColors(TevBlock.mTevKonstColors);

	glm::mat3x4 t[10];
	for (int i = 0; i < 10; i++)
		t[i] = glm::identity<glm::mat3x4>();

	J3DUniformBufferObject::SetTexMatrices(t);

	if (mShape != nullptr) {
		uint32_t offset, count;
		mShape->GetVertexOffsetAndCount(offset, count);

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (const void*)(offset * sizeof(uint16_t)));
	}

	glUseProgram(0);
	for (int i = 0; i < 8; i++)
		glBindTextureUnit(i, 0);
}
