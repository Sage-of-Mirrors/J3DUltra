#include "J3D/J3DFragmentShaderGenerator.hpp"
#include "J3D/J3DUtil.hpp"

#include <glad/glad.h>
#include <string>
#include <iostream>

bool J3DFragmentShaderGenerator::GenerateFragmentShader(uint32_t& shaderHandle) {
	// TODO: actual fragment shader generation

	shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

	std::string shaderChars = J3DUtility::LoadTextFile("./res/shaders/Debug_NormalColors.frag");
	const char* s = shaderChars.c_str();

	glShaderSource(shaderHandle, 1, &s, NULL);
	glCompileShader(shaderHandle);

	int32_t success = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::cout << "Fragment shader compilation failed!" << std::endl;
		return false;
	}

	return true;
}
