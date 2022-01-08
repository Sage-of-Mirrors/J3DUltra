#include "J3D/J3DFragmentShaderGenerator.hpp"

#include <glad/glad.h>
#include <string>
#include <iostream>

bool J3DFragmentShaderGenerator::GenerateFragmentShader(int32_t& shaderHandle) {
	// TODO: actual fragment shader generation
	std::string shaderStr = "";//LoadDebugShader("debug_normalcolors.frag");
	if (shaderStr.empty())
		return false;

	shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

	const char* shaderChars = shaderStr.c_str();
	glShaderSource(shaderHandle, 1, &shaderChars, NULL);

	glCompileShader(shaderHandle);

	int32_t success = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::cout << "Fragment shader compilation failed!" << std::endl;

		return false;
	}

	return true;
}
