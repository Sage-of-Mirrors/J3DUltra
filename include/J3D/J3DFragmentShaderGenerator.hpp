#pragma once

#include <cstdint>
#include <string>

class J3DMaterial;

class J3DFragmentShaderGenerator {
	static std::string GenerateIOVariables(J3DMaterial* material);
	static std::string GenerateStructs();
	static std::string GenerateUtilityFunctions();
	static std::string GenerateMainFunction(J3DMaterial* material);
	static std::string GenerateTEVStage(J3DMaterial* material, uint32_t index);
public:
	static bool GenerateFragmentShader(J3DMaterial* material, uint32_t& shaderHandle);
};
