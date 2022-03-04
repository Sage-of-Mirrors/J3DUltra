#pragma once

#include <cstdint>
#include <string>

class J3DMaterial;

class J3DFragmentShaderGenerator {
	static std::string GenerateTEVStage(J3DMaterial* material, uint32_t index);
public:
	static bool GenerateFragmentShader(J3DMaterial* material, uint32_t& shaderHandle);
};
