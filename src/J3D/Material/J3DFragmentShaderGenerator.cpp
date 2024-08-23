#include "J3D/Material/J3DFragmentShaderGenerator.hpp"

#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Material/J3DMaterialData.hpp"
#include "J3D/Material/J3DShaderGeneratorCommon.hpp"
#include "J3D/Material/J3DTEVTokens.hpp"
#include "J3D/Util/J3DUtil.hpp"
#include "GX/GXEnum.hpp"

#include <magic_enum.hpp>
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define etoi magic_enum::enum_integer

bool J3DFragmentShaderGenerator::GenerateFragmentShader(J3DMaterial* material, uint32_t& shaderHandle) {
	// TODO: actual fragment shader generation

	std::stringstream fragmentShader;
	fragmentShader << GenerateIOVariables(material);
	fragmentShader << J3DShaderGeneratorCommon::GenerateStructs();
	fragmentShader << GenerateUtilityFunctions();
	fragmentShader << GenerateMainFunction(material);

	std::ofstream debugFOut("E:/Github/Jekyll/shader/" + material->Name + "_frag.glsl");
	if (debugFOut.is_open()) {
		debugFOut << fragmentShader.str();
		debugFOut.close();
	}

	shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

	//std::string shaderChars = J3DUtility::LoadTextFile("./res/shaders/Debug_NormalColors.frag");
	std::string shaderStr = fragmentShader.str();
	const char* s = shaderStr.c_str();

	glShaderSource(shaderHandle, 1, &s, NULL);
	glCompileShader(shaderHandle);

	int32_t success = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::cout << "Fragment shader compilation failed!" << std::endl;
		
		GLsizei size = 0;
		char t[512];
		glGetShaderInfoLog(shaderHandle, 512, &size, t);

		std::cout << t << std::endl;

		return false;
	}

	return true;
}

std::string J3DFragmentShaderGenerator::GenerateIOVariables(J3DMaterial* material) {
	std::stringstream stream;

	stream << "#version 460\n\n";
	stream << "// Vertex shader outputs\n";
	stream << "in vec4 oColor0;\n";
	stream << "in vec4 oColor1;\n\n";

	uint32_t texGenCount = material->TexGenBlock.mTexCoordInfo.size();
	stream << "// Tex gen count: " << texGenCount << "\n";
	for (int i = 0; i < texGenCount; i++) {
		stream << "in vec3 oTexCoord" << i << ";\n";
	}

	stream << "\n// Final pixel color\n";
	stream << "out vec4 PixelColor;\n\n";

	stream << "// Texture\n";
	stream << "uniform sampler2D Texture[8];\n\n";

	return stream.str();
}

std::string J3DFragmentShaderGenerator::GenerateUtilityFunctions() {
	std::stringstream stream;

	// mix() function for combining two integers based on a third.
	stream << "int mix(int a, int b, int c) {\n";
	stream << "\ta = a & 0xFF;\n";
	stream << "\tb = b & 0xFF;\n";
	stream << "\tc = c & 0xFF;\n\n";
	stream << "\treturn clamp(a + ((b - a) * c / 255), 0, 255);\n";
	stream << "}\n\n";

	// Component-wise mix() for ivec3.
	stream << "ivec4 mix(ivec4 a, ivec4 b, ivec4 c) {\n";
	stream << "\tint red = mix(a.r, b.r, c.r);\n";
	stream << "\tint grn = mix(a.g, b.g, c.g);\n";
	stream << "\tint blu = mix(a.b, b.b, c.b);\n\n";
	stream << "\treturn ivec4(red, grn, blu, 0);\n";
	stream << "}\n\n";

	// Convert the given float to an int in the range 0..255.
	stream << "int FloatToS10(float f) {\n";
	stream << "\treturn int(f * 255.0) & 0xFF;\n";
	stream << "}\n\n";

	// Convert a float vec4 with components in range 0..1 to an integer vec4 with components 0..255.
	stream << "ivec4 VecFloatToS10(vec4 a) {\n";
	stream << "\treturn ivec4(FloatToS10(a.r), FloatToS10(a.g), FloatToS10(a.b), FloatToS10(a.a));\n";
	stream << "}\n\n";

	// Convert a float vec3 with components in range 0..1 to an integer vec4 with components 0..255.
	stream << "ivec3 VecFloatToS10(vec3 a) {\n";
	stream << "\treturn ivec3(FloatToS10(a.r), FloatToS10(a.g), FloatToS10(a.b));\n";
	stream << "}\n\n";

	// Convert an integer vec4 with components in range 0..255 to a float vec4 with components 0..1.
	stream << "vec4 VecS10ToFloat(ivec4 a) {\n";
	stream << "\tfloat red = (a.r & 0xFF) / 255.0;\n";
	stream << "\tfloat grn = (a.g & 0xFF) / 255.0;\n";
	stream << "\tfloat blu = (a.b & 0xFF) / 255.0;\n";
	stream << "\tfloat alf = (a.a & 0xFF) / 255.0;\n\n";
	stream << "\treturn vec4(red, grn, blu, alf);\n";
	stream << "}\n\n";

	// Combine the G and R values of the given vector into a single number. Used during color calculation.
	stream << "int CombineGR(ivec4 a) {\n";
	stream << "\treturn a.g << 8 | a.r;\n";
	stream << "}\n\n";

	// Combine the B, G, and R values of the given vector into a single number. Used during color calculation.
	stream << "int CombineBGR(ivec4 a) {\n";
	stream << "\treturn a.b << 16 | a.g << 8 | a.r;\n";
	stream << "}\n\n";

	// Compare the given vectors component-wise using greater-than. Used during color calculation.
	stream << "bool ComponentWiseGreater(ivec4 a, ivec4 b) {\n";
	stream << "\treturn a.r > b.r && a.g > b.g && a.b > b.b;\n";
	stream << "}\n\n";

	// Compare the given vectors component-wise using equals. Used during color calculation.
	stream << "bool ComponentWiseEquals(ivec4 a, ivec4 b) {\n";
	stream << "\treturn a.r == b.r && a.g == b.g && a.b == b.b;\n";
	stream << "}\n\n";

	stream << "float saturate(float v) {\n";
	stream << "\treturn clamp(v, 0.0, 1.0);";
	stream << "}\n\n";

	return stream.str();
}

std::string J3DFragmentShaderGenerator::GenerateMainFunction(J3DMaterial* material) {
	std::stringstream stream;
	stream << "void main() {\n";

	// The four locations that TEV stages can output to, initialized by the TevColor array.
	stream << "\tivec4 TevPrev = ivec4(TevColor[3]);\n";
	stream << "\tivec4 Reg0 = ivec4(TevColor[0]);\n";
	stream << "\tivec4 Reg1 = ivec4(TevColor[1]);\n";
	stream << "\tivec4 Reg2 = ivec4(TevColor[2]);\n\n";

	for (int i = 0; i < material->TEVStageGenMax; i++) {
		stream << GenerateTEVStage(material, i);
	}
	
	//if (material->PEBlock.mFog.Type != EGXFogType::None) {
	 	//stream << std::endl;
		//stream << GenerateFog(material->PEBlock.mFog);
	//}

	stream << "\n\tTevPrev = TevPrev & 0xFF;\n";

	stream << GenerateAlphaCompare(material->PEBlock.mAlphaCompare);
	stream << "\n\tPixelColor = VecS10ToFloat(TevPrev) + HighlightColor;\n";
	stream << "}\n";

	return stream.str();
}

bool IsIndirectEnabledForStage(std::shared_ptr<J3DIndirectTevStageInfo> indTevStage) {
	return indTevStage->TexMtxId != EGXIndirectTexMatrixId::IndTexMtx_Off;
}

std::string J3DFragmentShaderGenerator::GenerateTextureColor(J3DMaterial* material, uint32_t index) {
	std::shared_ptr<J3DTevOrderInfo> tevOrder = material->TevBlock->mTevOrders[index];
	std::stringstream stream;

	if (tevOrder->TexCoordId != EGXTexCoordSlot::Null) {
		std::string componentSwap = "rgba";
		componentSwap[0] = TGXTevSwapComponents[(uint8_t)tevOrder->mTexSwapMode.R];
		componentSwap[1] = TGXTevSwapComponents[(uint8_t)tevOrder->mTexSwapMode.G];
		componentSwap[2] = TGXTevSwapComponents[(uint8_t)tevOrder->mTexSwapMode.B];
		componentSwap[3] = TGXTevSwapComponents[(uint8_t)tevOrder->mTexSwapMode.A];

		if (material->IndirectBlock && material->IndirectBlock->mEnabled && IsIndirectEnabledForStage(material->IndirectBlock->mIndirectTevStages[index])) {
			std::shared_ptr<J3DIndirectTevStageInfo> indTevStage = material->IndirectBlock->mIndirectTevStages[index];
			uint32_t indStageIndex = etoi(indTevStage->TevStageId);
			std::shared_ptr<J3DIndirectTexOrderInfo> indTexOrder = material->IndirectBlock->mIndirectTexOrders[indStageIndex];

			stream << "\t\t// Indirect texturing enabled. Source coords: " << magic_enum::enum_name(indTexOrder->TexCoordId)
				<< ", source texture map: " << magic_enum::enum_name(indTexOrder->TexMapId) << "\n";
			
			stream << "\t\tvec2 BaseCoords = vec2((" << TGXTexCoordSlot[etoi(indTexOrder->TexCoordId)] << ".x / " << TGXTexCoordSlot[etoi(indTexOrder->TexCoordId)] << ".z) * " << TGXIndScale[etoi(material->IndirectBlock->mIndirectTexCoordScales[indStageIndex]->ScaleS)] << ", ("
				                                     << TGXTexCoordSlot[etoi(indTexOrder->TexCoordId)] << ".y / " << TGXTexCoordSlot[etoi(indTexOrder->TexCoordId)] << ".z) * " << TGXIndScale[etoi(material->IndirectBlock->mIndirectTexCoordScales[indStageIndex]->ScaleT)] << ");\n";
			stream << "\t\tivec4 IndLookupCoords = VecFloatToS10(vec4(texture(Texture[" << std::to_string(etoi(indTexOrder->TexMapId)) << "], BaseCoords).abg, 1.0));\n";

			if (indTevStage->TexBias != EGXIndirectTexBias::IndBias_None) {
				const char* biasVal = indTevStage->TexFormat == EGXIndirectTexFormat::IndFormat_8 ? "-128" : "1";
				stream << "\t\tIndLookupCoords += ivec4(";

				switch (indTevStage->TexBias) {
				case EGXIndirectTexBias::IndBias_S:
					stream << biasVal << ", 0, 0";
					break;
				case EGXIndirectTexBias::IndBias_ST:
					stream << biasVal << ", " << biasVal << ", 0";
					break;
				case EGXIndirectTexBias::IndBias_SU:
					stream << biasVal << ", 0, " << biasVal;
					break;
				case EGXIndirectTexBias::IndBias_T:
					stream << "0, " << biasVal << ", 0";
					break;
				case EGXIndirectTexBias::IndBias_TU:
					stream << "0, " << biasVal << ", " << biasVal;
					break;
				case EGXIndirectTexBias::IndBias_U:
					stream <<"0, 0, " << biasVal;
					break;
				case EGXIndirectTexBias::IndBias_STU:
					stream << biasVal << ", " << biasVal << ", " << biasVal;
					break;
				default:
					break;
				}

				stream << ", 0);\n\n";
			}

			stream << "\t\tvec4 FinalIndLookupCoords = ";

			switch (indTevStage->TexMtxId) {
				case EGXIndirectTexMatrixId::IndTexMtx_0:
				case EGXIndirectTexMatrixId::IndTexMtx_1:
				case EGXIndirectTexMatrixId::IndTexMtx_2:
					stream << "IndTexMatrices[" << etoi(indTevStage->TexMtxId) - 1 << "] * VecS10ToFloat(IndLookupCoords);\n";
					break;
				default:
					stream << "vec4(0.0);\n";
					break;
			}

			stream << "\n";
		}
		else {
			stream << "\t\t// No indirect texturing\n";
			stream << "\t\tvec4 FinalIndLookupCoords = vec4(0.0);\n\n";
		}

		stream << "\t\t// Texture Coords: " << magic_enum::enum_name(tevOrder->TexCoordId)
			<< ", Texture Map: " << magic_enum::enum_name(tevOrder->TexMapId) << ", Component Swap: " << componentSwap << "\n";

		stream << "\t\tvec3 ModifiedTexCoords = (" << TGXTexCoordSlot[etoi(tevOrder->TexCoordId)] << " / " << TGXTexCoordSlot[etoi(tevOrder->TexCoordId)] << ".z) + FinalIndLookupCoords.xyz;\n";

		stream << "\t\tivec4 TexTemp = VecFloatToS10(texture(Texture[" << std::to_string(etoi(tevOrder->TexMapId)) << "], ModifiedTexCoords.xy)." << componentSwap << ");\n\n";
	}
	else {
		stream << "\t\t// No texture specified per TEV Order.\n\n";
	}

	return stream.str();
}

std::string J3DFragmentShaderGenerator::GenerateRasterColor(std::shared_ptr<J3DTevOrderInfo> tevOrder) {
	std::stringstream stream;

	// Raster color
	if (tevOrder->ChannelId != EGXColorChannelId::ColorNull) {
		std::string componentSwap = "rgba";
		componentSwap[0] = TGXTevSwapComponents[(uint8_t)tevOrder->mRasSwapMode.R];
		componentSwap[1] = TGXTevSwapComponents[(uint8_t)tevOrder->mRasSwapMode.G];
		componentSwap[2] = TGXTevSwapComponents[(uint8_t)tevOrder->mRasSwapMode.B];
		componentSwap[3] = TGXTevSwapComponents[(uint8_t)tevOrder->mRasSwapMode.A];

		stream << "\t\t// Raster color source: " << magic_enum::enum_name(tevOrder->ChannelId) << ", Component Swap: " << componentSwap << "\n";
		stream << "\t\tivec4 RasTemp = ";

		switch (tevOrder->ChannelId) {
			case EGXColorChannelId::Color0:
			case EGXColorChannelId::Color1:
				stream << "ivec4(" << TGXTevColorChannelId[etoi(tevOrder->ChannelId)] << ", 1)." << componentSwap;
				break;
			case EGXColorChannelId::Color0A0:
			case EGXColorChannelId::Color1A1:
				stream << "VecFloatToS10(" << TGXTevColorChannelId[etoi(tevOrder->ChannelId)] << ")." << componentSwap;
				break;
			case EGXColorChannelId::ColorZero:
			default:
				stream << "ivec4(0, 0, 0, 0)";
				break;
		}

		stream << ";\n\n";
	}
	else {
		stream << "\t\t// No raster color specified per TEV Order.\n\n";
	}

	return stream.str();
}

std::string J3DFragmentShaderGenerator::GenerateKonstColor(EGXKonstColorSel colorSel, EGXKonstAlphaSel alphaSel) {
	std::stringstream stream;

	if (colorSel == EGXKonstColorSel::KCSel_Null || alphaSel == EGXKonstAlphaSel::KASel_Null) {
		stream << "\t\t// No valid konst color selections, so no konst color.\n\n";
		return stream.str();
	}

	// Konst color
	stream << "\t\t// Konst color source: " << magic_enum::enum_name(colorSel) << ", alpha source: " << magic_enum::enum_name(alphaSel) << "\n";

	uint32_t konstColorVal = etoi(colorSel);
	uint32_t konstAlphaVal = etoi(alphaSel);

	stream << "\t\tivec4 KonstTemp = ivec4(";

	if (konstColorVal <= etoi(EGXKonstColorSel::KCSel_1_8))
		stream << TGXKonstColorSel[konstColorVal] << ", ";
	else
		stream << "VecFloatToS10(" << TGXKonstColorSel[konstColorVal] << "), ";

	if (konstAlphaVal <= etoi(EGXKonstAlphaSel::KASel_1_8))
		stream << TGXKonstAlphaSel[konstAlphaVal] << ")";
	else
		stream << "FloatToS10(" << TGXKonstAlphaSel[konstAlphaVal] << "))";

	stream << ";\n\n";

	return stream.str();
}

std::string J3DFragmentShaderGenerator::GenerateColorCombiner(std::shared_ptr<J3DTevStageInfo> stage) {
	std::stringstream stream;

	// TEV color inputs
	stream << "\t\tivec4 Tev_C_A = " << TGXCombineColorInput[etoi(stage->ColorInput[0])] << ";\n";
	stream << "\t\tivec4 Tev_C_B = " << TGXCombineColorInput[etoi(stage->ColorInput[1])] << ";\n";
	stream << "\t\tivec4 Tev_C_C = " << TGXCombineColorInput[etoi(stage->ColorInput[2])] << ";\n";
	stream << "\t\tivec4 Tev_C_D = " << TGXCombineColorInput[etoi(stage->ColorInput[3])] << ";\n";

	stream << "\t\t" << TGXTevRegister[etoi(stage->ColorOutputRegister)] << ".rgb = ";
	
	std::stringstream tevCalcStream;

	switch (stage->ColorOperation) {
		case EGXTevOp::Add:
			tevCalcStream << "(Tev_C_D + mix(Tev_C_A, Tev_C_B, Tev_C_C)" << TGXTevBias[etoi(stage->ColorBias)] << ")" << TGXTevScale[etoi(stage->ColorScale)];
			break;
		case EGXTevOp::Sub:
			tevCalcStream << "(Tev_C_D - mix(Tev_C_A, Tev_C_B, Tev_C_C)" << TGXTevBias[etoi(stage->ColorBias)] << ")" << TGXTevScale[etoi(stage->ColorScale)];
			break;
		case EGXTevOp::Comp_R8_GT:
			tevCalcStream << "Tev_C_D + (Tev_C_A.r > Tev_C_B.r ? Tev_C_C : ivec4(0, 0, 0, 0))";
			break;
		case EGXTevOp::Comp_R8_EQ:
			tevCalcStream << "Tev_C_D + (Tev_C_A.r == Tev_C_B.r ? Tev_C_C : ivec4(0, 0, 0, 0))";
			break;
		case EGXTevOp::Comp_GR16_GT:
			tevCalcStream << "Tev_C_D + (CombineGR(Tev_C_A) > CombineGR(Tev_C_B) ? Tev_C_C : ivec4(0, 0, 0, 0))";
			break;
		case EGXTevOp::Comp_GR16_EQ:
			tevCalcStream << "Tev_C_D + (CombineGR(Tev_C_A) == CombineGR(Tev_C_B) ? Tev_C_C : ivec4(0, 0, 0, 0))";
			break;
		case EGXTevOp::Comp_BGR24_GT:
			tevCalcStream << "Tev_C_D + (CombineBGR(Tev_C_A) > CombineBGR(Tev_C_B) ? Tev_C_C : ivec4(0, 0, 0, 0))";
			break;
		case EGXTevOp::Comp_BGR24_EQ:
			tevCalcStream << "Tev_C_D + (CombineBGR(Tev_C_A) == CombineBGR(Tev_C_B) ? Tev_C_C : ivec4(0, 0, 0, 0))";
			break;
		case EGXTevOp::Comp_RGB8_GT:
			tevCalcStream << "Tev_C_D + (ComponentWiseGreater(Tev_C_A, Tev_C_B) ? Tev_C_C : ivec4(0, 0, 0, 0))";
			break;
		case EGXTevOp::Comp_RGB8_EQ:
			tevCalcStream << "Tev_C_D + (ComponentWiseEquals(Tev_C_A, Tev_C_B) ? Tev_C_C : ivec4(0, 0, 0, 0))";
			break;
		default:
			tevCalcStream << "ivec4(0, 0, 0, 0)";
			break;
	}

	if (stage->ColorClamp) {
		stream << "clamp(" << tevCalcStream.str() << ", 0, 255).rgb;\n";
	}
	else
		stream << "clamp(" << tevCalcStream.str() << ", -1024, 1023).rgb;\n";

	return stream.str();
}

std::string J3DFragmentShaderGenerator::GenerateAlphaCombiner(std::shared_ptr<J3DTevStageInfo> stage) {
	std::stringstream stream;

	// TEV color inputs
	stream << "\t\tivec4 Tev_A_A = " << TGXCombineAlphaInput[etoi(stage->AlphaInput[0])] << ";\n";
	stream << "\t\tivec4 Tev_A_B = " << TGXCombineAlphaInput[etoi(stage->AlphaInput[1])] << ";\n";
	stream << "\t\tivec4 Tev_A_C = " << TGXCombineAlphaInput[etoi(stage->AlphaInput[2])] << ";\n";
	stream << "\t\tivec4 Tev_A_D = " << TGXCombineAlphaInput[etoi(stage->AlphaInput[3])] << ";\n";

	stream << "\t\t" << TGXTevRegister[etoi(stage->AlphaOutputRegister)] << ".a = ";

	std::stringstream tevCalcStream;

	switch (stage->AlphaOperation) {
		case EGXTevOp::Add:
			tevCalcStream << "(Tev_A_D.a + mix(Tev_A_A.a, Tev_A_B.a, Tev_A_C.a)" << TGXTevBias[etoi(stage->AlphaBias)] << ")" << TGXTevScale[etoi(stage->AlphaScale)];
			break;
		case EGXTevOp::Sub:
			tevCalcStream << "(Tev_A_D.a - mix(Tev_A_A.a, Tev_A_B.a, Tev_A_C.a)" << TGXTevBias[etoi(stage->AlphaBias)] << ")" << TGXTevScale[etoi(stage->AlphaScale)];
			break;
		case EGXTevOp::Comp_R8_GT:
			tevCalcStream << "Tev_A_D.a + (Tev_A_A.r > Tev_A_B.r ? Tev_A_C.a : 0)";
			break;
		case EGXTevOp::Comp_R8_EQ:
			tevCalcStream << "Tev_A_D.a + (Tev_A_A.r == Tev_A_B.r ? Tev_A_C.a : 0)";
			break;
		case EGXTevOp::Comp_GR16_GT:
			tevCalcStream << "Tev_A_D.a + (CombineGR(Tev_A_A) > CombineGR(Tev_A_B) ? Tev_A_C.a : 0)";
			break;
		case EGXTevOp::Comp_GR16_EQ:
			tevCalcStream << "Tev_A_D.a + (CombineGR(Tev_A_A) == CombineGR(Tev_A_B) ? Tev_A_C.a : 0)";
			break;
		case EGXTevOp::Comp_BGR24_GT:
			tevCalcStream << "Tev_A_D.a + (CombineBGR(Tev_A_A) > CombineBGR(Tev_A_B) ? Tev_A_C.a : 0)";
			break;
		case EGXTevOp::Comp_BGR24_EQ:
			tevCalcStream << "Tev_A_D.a + (CombineBGR(Tev_A_A) == CombineBGR(Tev_A_B) ? Tev_A_C.a : 0)";
			break;
		case EGXTevOp::Comp_A8_GT:
			tevCalcStream << "Tev_A_D.a + (Tev_A_A.a > Tev_A_B.a ? Tev_A_C.a : 0)";
			break;
		case EGXTevOp::Comp_A8_EQ:
			tevCalcStream << "Tev_A_D.a + (Tev_A_A.a == Tev_A_B.a ? Tev_A_C.a : 0)";
			break;
		default:
			tevCalcStream << "0";
			break;
	}

	if (stage->AlphaClamp) {
		stream << "clamp(" << tevCalcStream.str() << ", 0, 255);\n";
	}
	else
		stream << "clamp(" << tevCalcStream.str() << ", -1024, 1023);\n";

	return stream.str();
}

std::string J3DFragmentShaderGenerator::GenerateTEVStage(J3DMaterial* material, uint32_t index) {
	std::shared_ptr<J3DTevOrderInfo> curTevOrder = material->TevBlock->mTevOrders[index];
	std::shared_ptr<J3DTevStageInfo> curTevStage = material->TevBlock->mTevStages[index];

	std::stringstream stream;
	stream << "\t// TEV Stage " << std::to_string(index) << "\n";
	stream << "\t{\n";

	stream << GenerateTextureColor(material, index);
	stream << GenerateRasterColor(curTevOrder);
	stream << GenerateKonstColor(material->TevBlock->mKonstColorSelection[index], material->TevBlock->mKonstAlphaSelection[index]);

	stream << GenerateColorCombiner(curTevStage);
	stream << GenerateAlphaCombiner(curTevStage);

	stream << "\t}\n";
	return stream.str();
}

std::string GenerateAlphaCompareComponent(EGXCompareType compareType, uint8_t ref) {
	std::stringstream stream;

	switch (compareType) {
		case EGXCompareType::Always:
			stream << "true";
			break;
		case EGXCompareType::Equal:
			stream << "TevPrev.a == " << std::to_string(ref);
			break;
		case EGXCompareType::GEqual:
			stream << "TevPrev.a >= " << std::to_string(ref);
			break;
		case EGXCompareType::Greater:
			stream << "TevPrev.a > " << std::to_string(ref);
			break;
		case EGXCompareType::LEqual:
			stream << "TevPrev.a <= " << std::to_string(ref);
			break;
		case EGXCompareType::Less:
			stream << "TevPrev.a < " << std::to_string(ref);
			break;
		case EGXCompareType::NEqual:
			stream << "TevPrev.a != " << std::to_string(ref);
			break;
		case EGXCompareType::Never:
			stream << "false";
			break;
	}

	return stream.str();
}

std::string J3DFragmentShaderGenerator::GenerateAlphaCompare(J3DAlphaCompare& alphaCompare) {
	std::stringstream stream;

	std::string compare0 = GenerateAlphaCompareComponent(alphaCompare.CompareFunc0, alphaCompare.Reference0);
	std::string compare1 = GenerateAlphaCompareComponent(alphaCompare.CompareFunc1, alphaCompare.Reference1);

	stream << "\n\tif (!(";

	switch (alphaCompare.Operation) {
		case EGXAlphaOp::And:
			stream << "(" << compare0 << ") && (" << compare1 << ")";
			break;
		case EGXAlphaOp::Or:
			stream << "(" << compare0 << ") || (" << compare1 << ")";
			break;
		case EGXAlphaOp::XNOR:
			stream << compare0 << " == " << compare1;
			break;
		case EGXAlphaOp::XOR:
			stream << compare0 << " != " << compare1;
			break;
	}

	stream << ")) {\n";
	stream << "\t\tdiscard;\n";
	stream << "\t}\n";

	return stream.str();
}

std::string J3DFragmentShaderGenerator::GenerateFog(J3DFog& fog) {
	std::stringstream stream;

	stream << "\tfloat fogA = " << (1000000.f * 1.0f) / ((1000000.f - 1.0f) * (fog.EndZ - fog.StartZ)) << ";\n";
	stream << "\tfloat fogB = " << 1000000.f / (1000000.f - 1.0f) << ";\n";
	stream << "\tfloat fogC = " << fog.StartZ / (fog.EndZ - fog.StartZ) << ";\n\n";

	stream << "\tfloat FogBase = fogA  / (fogB - gl_FragCoord.z);\n";
	stream << "\tfloat FogZ = saturate(FogBase - fogC);\n";
	stream << "\tfloat Fog = ";

	switch (fog.Type) {
		case EGXFogType::Linear:
			stream << "FogZ";
			break;
		case EGXFogType::Exponential:
			stream << "1.0 - exp2(-8.0 * FogZ)";
			break;
		case EGXFogType::Exponential_2:
			stream << "1.0 - exp2(-8.0 * FogZ * FogZ)";
			break;
		case EGXFogType::Reverse_Exponential:
			stream << "1.0 - exp2(-8.0 * (1.0 - FogZ))";
			break;
		case EGXFogType::Reverse_Exponential_2:
			stream << "1.0 - exp2(-8.0 * (1.0 - FogZ) * (1.0 - FogZ))";
			break;
		case EGXFogType::None:
		default:
			stream << "0";
			break;
	}

	stream << ";\n\n";
	stream << "\tTevPrev = mix(TevPrev.rgba, ivec4(" << fog.Color.r << ", " << fog.Color.g << ", " << fog.Color.b << ", " << fog.Color.a << "), ivec4(FloatToS10(Fog), FloatToS10(Fog), FloatToS10(Fog), FloatToS10(Fog))); \n";

	return stream.str();
}
