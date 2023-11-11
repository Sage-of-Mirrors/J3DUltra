#pragma once

#include "GX/GXEnum.hpp"

#include <vector>
#include <cstdint>
#include <string>
#include <memory>

enum class EGXAttribute : uint32_t;
class J3DMaterial;
struct J3DTexGenBlock;
struct J3DLightBlock;
struct J3DTexCoordInfo;
struct J3DColorChannel;

class J3DVertexShaderGenerator {
	static std::string GenerateAttributes(const J3DMaterial* material);
	static std::string GenerateOutputs(const J3DMaterial* material);
	static std::string GenerateUniforms();
	static std::string GenerateLight(std::shared_ptr<J3DColorChannel> colorChannel, const uint32_t& lightIndex);
	static std::string GenerateColorChannel(std::shared_ptr<J3DColorChannel> colorChannnel, const int32_t& index);
	static std::string GenerateTexGen(std::shared_ptr<J3DTexCoordInfo> texGen, const uint32_t index);
	static std::string GenerateMainFunction(const J3DMaterial* material, const bool hasNormals);

	static bool IsAttributeUsed(EGXAttribute a, const J3DMaterial* material);
public:
	static bool GenerateVertexShader(const J3DMaterial* material, uint32_t& shaderHandle);
};
