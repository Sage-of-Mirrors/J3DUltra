#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>

enum class EGLAttribute : uint32_t;
class J3DMaterial;

class J3DVertexShaderGenerator {
	static void WriteAttributes(std::stringstream& shaderTxt, const std::vector<EGLAttribute>& shapeAttributes);
	static void WriteOutputs(std::stringstream& shaderTxt, const J3DMaterial* material);
	static void WriteUniforms(std::stringstream& shaderTxt, const int32_t& jointCount);
	static void WriteSkinningFunction(std::stringstream& shaderTxt);
	static void WriteMainFunction(std::stringstream& shaderTxt, const J3DMaterial* material);
public:
	static bool GenerateVertexShader(const J3DMaterial* material, const int32_t& jointCount, uint32_t& shaderHandle);
};
