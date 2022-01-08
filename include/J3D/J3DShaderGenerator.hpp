#pragma once

enum class EGLAttribute;

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>

class J3DShaderGenerator {
	static std::string LoadDebugShader(std::string filename);
	static void WriteAttributes(std::stringstream& shaderTxt, const std::vector<EGLAttribute>& shapeAttributes);
	static void WriteSkinningFunction(std::stringstream& shaderTxt);
public:
	static bool GenerateVertexShader(const std::vector<EGLAttribute>& shapeAttributes, int32_t& shaderHandle);
	static bool GenerateFragmentShader(int32_t& shaderHandle);
};
