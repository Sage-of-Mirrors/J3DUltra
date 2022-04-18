#pragma once

#include "GX/GXEnum.hpp"

#include <vector>
#include <cstdint>
#include <string>

enum class EGXAttribute : uint32_t;
class J3DMaterial;
struct J3DTexGenBlock;
struct J3DLightBlock;
struct J3DTexCoordInfo;
struct J3DColorChannel;

class J3DVertexShaderGenerator {
	static std::string GenerateAttributes(const std::vector<EGXAttribute>& shapeAttributes);
	static std::string GenerateOutputs(const J3DMaterial* material);
	static std::string GenerateUniforms();
	static std::string GenerateLight(const J3DColorChannel& colorChannel, const uint32_t& lightIndex);
	static std::string GenerateColorChannel(const J3DColorChannel& colorChannnel, const int32_t& index);
	static std::string GenerateTexGen(const J3DTexCoordInfo& texGen, const uint32_t index);
	static std::string GenerateMainFunction(const J3DMaterial* material);
public:
	static bool GenerateVertexShader(const J3DMaterial* material, const int32_t& jointCount, uint32_t& shaderHandle);
};
