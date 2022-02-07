#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>

enum class EGLAttribute : uint32_t;
class J3DMaterial;
struct J3DTexGenBlock;
struct J3DLightBlock;

class J3DVertexShaderGenerator {
	static void WriteAttributes(std::stringstream& shaderTxt, const std::vector<EGLAttribute>& shapeAttributes);
	static void WriteOutputs(std::stringstream& shaderTxt, const J3DMaterial* material);
	static void WriteUniforms(std::stringstream& shaderTxt, const int32_t& jointCount);
	static void WriteMainFunction(std::stringstream& shaderTxt, const J3DMaterial* material);
	static void WriteTexGen(std::stringstream& shaderTxt, const J3DTexGenBlock& texGenBlock, const int32_t& index);
	static void WriteColorChannels(std::stringstream& shaderTxt, const J3DLightBlock& lightBlock, const int32_t& index);
public:
	static bool GenerateVertexShader(const J3DMaterial* material, const int32_t& jointCount, uint32_t& shaderHandle);
};
