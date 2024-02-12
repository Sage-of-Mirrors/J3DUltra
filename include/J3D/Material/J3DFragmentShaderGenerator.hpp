#pragma once

#include <cstdint>
#include <string>
#include <memory>

enum class EGXKonstColorSel : uint8_t;
enum class EGXKonstAlphaSel : uint8_t;
class J3DMaterial;
struct J3DTevOrderInfo;
struct J3DTevStageInfo;
struct J3DAlphaCompare;
struct J3DFog;

class J3DFragmentShaderGenerator {
	static std::string GenerateIOVariables(J3DMaterial* material);
	static std::string GenerateUtilityFunctions();
	static std::string GenerateMainFunction(J3DMaterial* material);
	
	// TEV stuff
	static std::string GenerateTextureColor(J3DMaterial* material, uint32_t index);
	static std::string GenerateRasterColor(std::shared_ptr<J3DTevOrderInfo> tevOrder);
	static std::string GenerateKonstColor(EGXKonstColorSel colorSel, EGXKonstAlphaSel alphaSel);
	static std::string GenerateTEVStage(J3DMaterial* material, uint32_t index);
	static std::string GenerateColorCombiner(std::shared_ptr<J3DTevStageInfo> stage);
	static std::string GenerateAlphaCombiner(std::shared_ptr<J3DTevStageInfo> stage);
	static std::string GenerateAlphaCompare(J3DAlphaCompare& alphaCompare);
	static std::string GenerateFog(J3DFog& fog);
public:
	static bool GenerateFragmentShader(J3DMaterial* material, uint32_t& shaderHandle);
};
