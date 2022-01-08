#pragma once

enum class EGLAttribute;
class J3DMaterial;

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>

class J3DVertexShaderGenerator {
	static std::string LoadDebugShader(std::string filename);
	static void WriteAttributes(std::stringstream& shaderTxt, const std::vector<EGLAttribute>& shapeAttributes);
	static void WriteOutputs(std::stringstream& shaderTxt, const J3DMaterial* material);
	static void WriteUniforms(std::stringstream& shaderTxt, const int32_t& jointCount);
	static void WriteSkinningFunction(std::stringstream& shaderTxt);
	static void WriteMainFunction(std::stringstream& shaderTxt, const J3DMaterial* material);
public:
	static bool GenerateVertexShader(const J3DMaterial* material, const int32_t& jointCount, int32_t& shaderHandle);
};
