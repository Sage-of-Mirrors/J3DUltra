#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DShaderGenerator.hpp"
#include "J3D/J3DShape.hpp"

#include <iostream>
#include <vector>
#include <glad/glad.h>

J3DMaterial::J3DMaterial() : mShaderProgram(-1) {

}

J3DMaterial::~J3DMaterial() {
	if (mShaderProgram != -1)
		glDeleteProgram(mShaderProgram);
}

void J3DMaterial::Bind() {
	if (mShaderProgram != -1)
		glUseProgram(mShaderProgram);
	else
		std::cout << "Attempted to use invalid shader program on material " << mName << std::endl;
}

void J3DMaterial::Unbind() {
	glUseProgram(0);
}

bool J3DMaterial::GenerateShaders() {
	int32_t vertShader, fragShader;

	if (!J3DShaderGenerator::GenerateVertexShader(mShape->GetEnabledAttributes(), vertShader)) {
		std::cout << "Error in vertex shader generator!" << std::endl;
		return false;
	}

	if (!J3DShaderGenerator::GenerateFragmentShader(fragShader)) {
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
		std::cout << "Shader program for material " << mName << " failed to link. Error is as follows:" << std::endl;

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

	// Program linked successfully, detach and delete the shaders because they're not needed now
	glDetachShader(mShaderProgram, vertShader);
	glDetachShader(mShaderProgram, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return true;
}
