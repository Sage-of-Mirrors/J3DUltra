#include "J3D/J3DFragmentShaderGenerator.hpp"
#include "J3D/J3DUtil.hpp"
#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DMaterialData.hpp"
#include "../magic_enum/include/magic_enum.hpp"
#include "GX/GXEnum.hpp"
#include "J3D/J3DTEVTokens.hpp"

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define etoi magic_enum::enum_integer

bool J3DFragmentShaderGenerator::GenerateFragmentShader(J3DMaterial* material, uint32_t& shaderHandle) {
	// TODO: actual fragment shader generation

	std::stringstream fragmentShader;

	fragmentShader << "int mix(int a, int b, int c) {\n";
	fragmentShader << "\ta = a & 0xFF;\n";
	fragmentShader << "\tb = b & 0xFF;\n";
	fragmentShader << "\tc = c & 0xFF;\n\n";
	fragmentShader << "\treturn a + ((b - a) * c / 255);\n";
	fragmentShader << "}\n\n";

	fragmentShader << "ivec3 mix(ivec3 a, ivec3 b, ivec3 c) {\n";
	fragmentShader << "\tint r = mix(a.r, b.r, c.r);\n";
	fragmentShader << "\tint g = mix(a.g, b.g, c.g);\n";
	fragmentShader << "\tint b = mix(a.b, b.b, c.b);\n\n";
	fragmentShader << "\treturn ivec3(r, g, b);\n";
	fragmentShader << "}\n\n";

	fragmentShader << "ivec4 FloatToS10(vec4 a) {\n";
	fragmentShader << "\tint r = (a.r * 255) & 0xFF;\n";
	fragmentShader << "\tint g = (a.g * 255) & 0xFF;\n";
	fragmentShader << "\tint b = (a.b * 255) & 0xFF;\n";
	fragmentShader << "\tint a = (a.a * 255) & 0xFF;\n\n";
	fragmentShader << "\treturn ivec4(r, g, b, a);\n";
	fragmentShader << "}\n\n";

	fragmentShader << "vec4 S10ToFloat(ivec4 a) {\n";
	fragmentShader << "\tfloat r = (a.r & 0xFF) / 255.0;\n";
	fragmentShader << "\tfloat g = (a.g & 0xFF) / 255.0;\n";
	fragmentShader << "\tfloat b = (a.b & 0xFF) / 255.0;\n";
	fragmentShader << "\tfloat a = (a.a & 0xFF) / 255.0;\n\n";
	fragmentShader << "\treturn vec4(r, g, b, a);\n";
	fragmentShader << "}\n\n";

	fragmentShader << "void main() {\n";
	fragmentShader << "\tivec4 TevPrev = FloatToS10(TevColor[3]);\n";
	fragmentShader << "\tivec4 Reg0 = FloatToS10(TevColor[0]);\n";
	fragmentShader << "\tivec4 Reg1 = FloatToS10(TevColor[1]);\n";
	fragmentShader << "\tivec4 Reg2 = FloatToS10(TevColor[2]);\n\n";

	for (int i = 0; i < material->TevBlock.mTevStages.size(); i++) {
		fragmentShader << GenerateTEVStage(material, i);
	}

	fragmentShader << "\n\tPixelColor = S10ToFloat(TevPrev);\n";
	fragmentShader << "}\n";

	std::ofstream debugFOut("./shader/" + material->Name + "_frag.glsl");
	if (debugFOut.is_open()) {
		debugFOut << fragmentShader.str();
		debugFOut.close();
	}

	shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

	std::string shaderChars = J3DUtility::LoadTextFile("./res/shaders/Debug_NormalColors.frag");
	const char* s = shaderChars.c_str();

	glShaderSource(shaderHandle, 1, &s, NULL);
	glCompileShader(shaderHandle);

	int32_t success = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::cout << "Fragment shader compilation failed!" << std::endl;
		return false;
	}

	return true;
}

std::string J3DFragmentShaderGenerator::GenerateTEVStage(J3DMaterial* material, uint32_t index) {
	J3DTevOrderInfo& curTevOrder = material->TevBlock.mTevOrders[index];
	J3DTevStageInfo& curTevStage = material->TevBlock.mTevStages[index];

	std::stringstream stream;
	stream << "\t// TEV Stage " << std::to_string(index) << "\n";
	stream << "\t{\n";

	// Texture color
	if (curTevOrder.TexCoordId != EGXTexCoordSlot::Null) {
		std::string componentSwap = "rgba";
		for (int i = 0; i < 4; i++)
			componentSwap[i] = TGXTevSwapComponents[curTevOrder.mTexSwapTable[i]];

		stream << "\t\t// Texture Coords: " << magic_enum::enum_name(curTevOrder.TexCoordId)
			<< ", Texture Map: " << std::to_string(curTevOrder.TexMap) << ", Component Swap: " << componentSwap << "\n";

		stream << "\t\tivec4 TexTemp = FloatToS10(texture(Texture[" << std::to_string(curTevOrder.TexMap) << "], "
			<< TGXTexCoordSlot[etoi(curTevOrder.TexCoordId)] << ")." << componentSwap << ");\n";
	}
	else {
		stream << "\t\t// No texture specified per TEV Order.\n";
	}

	// Raster color
	if (curTevOrder.ChannelId != EGXColorChannelId::ColorNull) {
		std::string componentSwap = "rgba";
		for (int i = 0; i < 4; i++)
			componentSwap[i] = TGXTevSwapComponents[curTevOrder.mRasSwapTable[i]];

		stream << "\t\t// Raster color source: " << magic_enum::enum_name(curTevOrder.ChannelId) << ", Component Swap: " << componentSwap << "\n";
		stream << "\t\tivec4 RasTemp = ";

		if (curTevOrder.ChannelId == EGXColorChannelId::Color0 || curTevOrder.ChannelId == EGXColorChannelId::Color1) {
			stream << "FloatToS10((" << TGXTevColorChannelId[etoi(curTevOrder.ChannelId)] << ", 1.0).";
		}
		else {
			stream << TGXTevColorChannelId[etoi(curTevOrder.ChannelId)] << ".";
		}

		stream << componentSwap << ";\n";
	}
	else {
		stream << "\t\t// No raster color specified per TEV Order.\n";
	}

	// Konst color
	stream << "\t\t// Konst color source: " << magic_enum::enum_name(material->TevBlock.mKonstColorSelection[index]);
	stream << ", alpha source: " << magic_enum::enum_name(material->TevBlock.mKonstAlphaSelection[index]) << "\n";

	stream << "\t\tivec4 KonstTemp = FloatToS10(vec4(" << TGXKonstColorSel[etoi(material->TevBlock.mKonstColorSelection[index])];
	stream << ", " << TGXKonstAlphaSel[etoi(material->TevBlock.mKonstAlphaSelection[index])] << "));\n\n";

	// TEV color inputs
	stream << "\t\tivec3 Tev_C_A = " << TGXCombineColorInput[etoi(curTevStage.ColorInput[0])] << ";\n";
	stream << "\t\tivec3 Tev_C_B = " << TGXCombineColorInput[etoi(curTevStage.ColorInput[1])] << ";\n";
	stream << "\t\tivec3 Tev_C_C = " << TGXCombineColorInput[etoi(curTevStage.ColorInput[2])] << ";\n";
	stream << "\t\tivec3 Tev_C_D = " << TGXCombineColorInput[etoi(curTevStage.ColorInput[3])] << ";\n";

	stream << "\t\t" << TGXTevRegister[etoi(curTevStage.ColorOutputRegister)] << ".rgb = ";

	std::stringstream tevCalcStream;

	switch (curTevStage.ColorOperation) {
		case EGXTevOp::Add:
			tevCalcStream << "(Tev_C_D + mix(Tev_C_A, Tev_C_B, Tev_C_C)" << TGXTevBias[etoi(curTevStage.ColorBias)] << ")" << TGXTevScale[etoi(curTevStage.ColorScale)];
			break;
		case EGXTevOp::Sub:
			tevCalcStream << "(Tev_C_D - mix(Tev_C_A, Tev_C_B, Tev_C_C)" << TGXTevBias[etoi(curTevStage.ColorBias)] << ")" << TGXTevScale[etoi(curTevStage.ColorScale)];
			break;
	}

	if (curTevStage.ColorClamp) {
		stream << "clamp(" << tevCalcStream.str() << ", 0, 255);\n";
	}
	else
		stream << tevCalcStream.str() << ";\n";

	tevCalcStream.str(std::string());

	// TEV alpha inputs
	stream << "\n\t\tint Tev_A_A = " << TGXCombineAlphaInput[etoi(curTevStage.AlphaInput[0])] << ";\n";
	stream << "\t\tint Tev_A_B = " << TGXCombineAlphaInput[etoi(curTevStage.AlphaInput[1])] << ";\n";
	stream << "\t\tint Tev_A_C = " << TGXCombineAlphaInput[etoi(curTevStage.AlphaInput[2])] << ";\n";
	stream << "\t\tint Tev_A_D = " << TGXCombineAlphaInput[etoi(curTevStage.AlphaInput[3])] << ";\n";

	stream << "\t\t" << TGXTevRegister[etoi(curTevStage.AlphaOutputRegister)] << ".a = ";

	switch (curTevStage.AlphaOperation) {
	case EGXTevOp::Add:
		tevCalcStream << "(Tev_A_D + mix(Tev_A_A, Tev_A_B, Tev_A_C)" << TGXTevBias[etoi(curTevStage.AlphaBias)] << ")" << TGXTevScale[etoi(curTevStage.AlphaScale)];
		break;
	case EGXTevOp::Sub:
		tevCalcStream << "(Tev_A_D - mix(Tev_A_A, Tev_A_B, Tev_A_C)" << TGXTevBias[etoi(curTevStage.AlphaBias)] << ")" << TGXTevScale[etoi(curTevStage.AlphaScale)];
		break;
	}

	if (curTevStage.AlphaClamp) {
		stream << "clamp(" << tevCalcStream.str() << ", 0, 255);\n";
	}
	else
		stream << tevCalcStream.str() << ";\n";

	stream << "\t}\n";
	return stream.str();
}
