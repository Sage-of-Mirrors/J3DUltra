#include "J3D/J3DVertexShaderGenerator.hpp"
#include "J3D/J3DMaterialData.hpp"
#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DShape.hpp"
#include "J3D/J3DUtil.hpp"
#include "../lib/J3DUltra/magic_enum/include/magic_enum.hpp"

#include <glad/glad.h>

// For debugging
#include <iostream>
#include <fstream>

bool J3DVertexShaderGenerator::GenerateVertexShader(const J3DMaterial* material, const int32_t& jointCount, uint32_t& shaderHandle) {
	if (material == nullptr || material->GetShape() == nullptr)
		return false;
	
	shaderHandle = glCreateShader(GL_VERTEX_SHADER);

	std::stringstream shaderStream;
	shaderStream << "#version 330 core\n\n";

	//WriteAttributes(shaderStream, material->GetShape()->GetEnabledAttributes());
	//WriteOutputs(shaderStream, material);
	//WriteUniforms(shaderStream, jointCount);

	shaderStream << "float ApplyAttenuation(vec3 t_Coeff, float t_Value) {\n"
		"\treturn dot(t_Coeff, vec3(1.0, t_Value, t_Value * t_Value));\n"
		"}\n\n";

	//WriteMainFunction(shaderStream, material);

	shaderHandle = glCreateShader(GL_VERTEX_SHADER);
	
	std::string shaderStr = shaderStream.str();

	std::ofstream debugVOut("./shader/" + material->Name + "_vtx.glsl");
	if (debugVOut.is_open()) {
		debugVOut << shaderStr;
		debugVOut.close();
	}

	std::string shaderChars = J3DUtility::LoadTextFile("./res/shaders/Debug_NormalColors.vert");
	const char* s = shaderChars.c_str();

	glShaderSource(shaderHandle, 1, &s, NULL);
	glCompileShader(shaderHandle);

	int32_t success = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::cout << "Vertex shader compilation failed!" << std::endl;

		return false;
	}

	return true;
}

void J3DVertexShaderGenerator::WriteAttributes(std::stringstream& shaderTxt, const std::vector<EGLAttribute>& shapeAttributes) {
	shaderTxt << "// Input attributes\n";

	for (auto a : shapeAttributes) {
		if (a == EGLAttribute::PositionMatrixIdx)
			continue;

		shaderTxt << "layout (location = " << (uint32_t)a << ") in ";

		switch (a) {
			case EGLAttribute::Position:
				shaderTxt << "vec4 aPos;\n";
				break;
			case EGLAttribute::Normal:
				shaderTxt << "vec3 aNrm;\n";
				break;
			case EGLAttribute::Color0:
			case EGLAttribute::Color1:
				shaderTxt << "vec4 aCol" << (uint32_t)a - (uint32_t)EGLAttribute::Color0 << ";\n";
				break;
			case EGLAttribute::TexCoord0:
			case EGLAttribute::TexCoord1:
			case EGLAttribute::TexCoord2:
			case EGLAttribute::TexCoord3:
			case EGLAttribute::TexCoord4:
			case EGLAttribute::TexCoord5:
			case EGLAttribute::TexCoord6:
			case EGLAttribute::TexCoord7:
				shaderTxt << "vec3 aTex" << (uint32_t)a - (uint32_t)EGLAttribute::TexCoord0 << ";\n";
				break;
		}
	}

	shaderTxt << "\n";
}

void J3DVertexShaderGenerator::WriteOutputs(std::stringstream& shaderTxt, const J3DMaterial* material) {
	shaderTxt << "// Vertex shader outputs\n";

	//shaderTxt << "// Number of color channel controls: " << material->ChannelControlCount << "\n";
	shaderTxt << "out vec4 oColor[2];\n\n";

	uint32_t texGenCount = material->TexGenBlock.mTexCoordInfo.size();
	shaderTxt << "// Tex gen count: " << texGenCount << "\n";
	for (int i = 0; i < texGenCount; i++) {
		shaderTxt << "out vec3 oTexCoord" << i << ";\n";
	}

	shaderTxt << "\n";
}

void J3DVertexShaderGenerator::WriteUniforms(std::stringstream& shaderTxt, const int32_t& jointCount) {
	shaderTxt << "// Represents a hardware light source.\n";
	shaderTxt << "struct GXLight {\n"
			"\tvec4 Position;\n"
			"\tvec4 Direction;\n"
			"\tvec4 Color;\n"
			"\tvec4 AngleAtten;\n"
			"\tvec4 DistAtten;\n"
		"};\n\n";

	shaderTxt << "// This UBO contains data that doesn't change between vertices or materials.\n";
	shaderTxt << "layout (std140) uniform uSharedData {\n"
			"\tmat4 Proj;\n"
			"\tmat4 View;\n"
			"\tmat4 Model;\n\n"
		    "\tGXLight Lights[8];\n"
			"\tmat4 Envelopes[256];\n"
			"\tmat3x4 TexMatrices[10];\n"
		"};\n\n";

	shaderTxt << "// These uniforms can be modified per-material, usually by external animations.\n";
	shaderTxt << "uniform vec4 uMaterialReg[2];\n";
	shaderTxt << "uniform vec4 uAmbientReg[2];\n\n";
}

void J3DVertexShaderGenerator::WriteMainFunction(std::stringstream& shaderTxt, const J3DMaterial* material) {
	shaderTxt << "// Main shader program\n";

	// Start of main function
	shaderTxt << "void main() {\n";

	// Position calculations
	shaderTxt << "\tvec3 skinnedNormal = normalize(inverse(transpose(mat3(EnvelopeMatrices[int(aPos.w)]))) * aNrm);\n"
		"\toColor0 = vec4(skinnedNormal.rgb, 1);\n\n"
		"\tmat4 MVP = Proj * View * Model;\n"
		"\tgl_Position = MVP * (Envelopes[int(aPos.w)]) * vec4(aPos.xyz, 1);\n\n";

	shaderTxt << "\tvec4 lightAccumulator;\n"
		"\tvec3 lightDelta, lightDeltaDir\n"
		"\tfloat lightDeltaDist2, lightDeltaDist, attenuation;\n\n";

	for (int i = 0; i < material->LightBlock.mColorChannels.size(); i++)
		WriteColorChannels(shaderTxt, material->LightBlock, i);

	for (int i = 0; i < material->TexGenBlock.mTexCoordInfo.size(); i++)
		WriteTexGen(shaderTxt, material->TexGenBlock, i);

	// End of main function
	shaderTxt << "}\n";
}

void J3DVertexShaderGenerator::WriteTexGen(std::stringstream& shaderTxt, const J3DTexGenBlock& texGenBlock, const int32_t& index) {
	const J3DTexCoordInfo& texGen = texGenBlock.mTexCoordInfo[index];

	shaderTxt << "\t// TexGen " << index << " ("
		"Source: " << magic_enum::enum_name(texGen.Source) << ", "
		"Type: " << magic_enum::enum_name(texGen.Type) << ", "
		"TexMatrix: " << magic_enum::enum_name(texGen.TexMatrix) << ")\n";

	// Figure out where the initial tex coord is coming from
	std::string source = "";
	switch (texGen.Source) {
		case EGXTexGenSrc::Position:
			source = "aPos";
			break;
		case EGXTexGenSrc::Normal:
			source = "aNrm";
			break;
		case EGXTexGenSrc::Binormal:
			source = "aBin";
			break;
		case EGXTexGenSrc::Tangent:
			source = "aTan";
			break;
		case EGXTexGenSrc::Color0:
			source = "aCol0";
			break;
		case EGXTexGenSrc::Color1:
			source = "aCol1";
			break;
		case EGXTexGenSrc::Tex0:
		case EGXTexGenSrc::Tex1:
		case EGXTexGenSrc::Tex2:
		case EGXTexGenSrc::Tex3:
		case EGXTexGenSrc::Tex4:
		case EGXTexGenSrc::Tex5:
		case EGXTexGenSrc::Tex6:
		case EGXTexGenSrc::Tex7:
		{
			uint32_t texIndex = (uint32_t)texGen.Source - (uint32_t)EGXTexGenSrc::Tex0;
			source = "aTex" + std::to_string(texIndex);
			break;
		}
		case EGXTexGenSrc::TexCoord0:
		case EGXTexGenSrc::TexCoord1:
		case EGXTexGenSrc::TexCoord2:
		case EGXTexGenSrc::TexCoord3:
		case EGXTexGenSrc::TexCoord4:
		case EGXTexGenSrc::TexCoord5:
		case EGXTexGenSrc::TexCoord6:
		{
			uint32_t texCoordIndex = (uint32_t)texGen.Source - (uint32_t)EGXTexGenSrc::TexCoord0;
			source = "vec4(oTexCoord" + std::to_string(texCoordIndex) + ".xy, 1, 1)";
			break;
		}
	}

	std::string dest = "oTexCoord" + std::to_string(index);

	if (texGen.TexMatrix == EGXTexMatrix::Identity) {
		shaderTxt << "\t" << dest << " = " << source << ".xyz;\n";
	}
	else {
		uint32_t texMatrixIndex = ((uint32_t)texGen.TexMatrix - (uint32_t)EGXTexMatrix::TexMtx0) / 3;

		switch (texGen.Type) {
			case EGXTexGenType::Matrix2x4:
				shaderTxt << "\t" << dest << " = TexMatrices[" << texMatrixIndex << "] * vec3(" << source << ".xy, 1);\n";
				break;
			case EGXTexGenType::Matrix3x4:
				shaderTxt << "\t" << dest << " = TexMatrices[" << texMatrixIndex << "] * " << source << ".xyz; \n";
				break;
			case EGXTexGenType::SRTG:
				shaderTxt << "\t" << dest << " = " << "vec3(" << source << ".rg, 1);\n";
				break;
			default:
				shaderTxt << "\t" << dest << " = " << source << ".xyz;\n";
				break;
		}
	}
}

void J3DVertexShaderGenerator::WriteColorChannels(std::stringstream& shaderTxt, const J3DLightBlock& lightBlock, const int32_t& index) {
	const J3DColorChannel& colorChannel = lightBlock.mColorChannels[index];

	shaderTxt << "\t// Color channel " << index << " ("
		"Lighting enabled: " << std::to_string(colorChannel.LightingEnabled) << ", "
		"Material Source: " << magic_enum::enum_name(colorChannel.MaterialSource) << ", "
		"Light Mask: " << std::to_string(colorChannel.LightMask) << ", "
		"Diffuse Function: " << magic_enum::enum_name(colorChannel.DiffuseFunction) << ", "
		"Atten Function: " << magic_enum::enum_name(colorChannel.AttenuationFunction) << ", "
		"Ambient Source: " << magic_enum::enum_name(colorChannel.AmbientSource) << ")\n";

	EGXColorChannelId chanId = magic_enum::enum_value<EGXColorChannelId>(index);
	std::string colorDestination = "";
	std::string compDestination = "";

	switch (chanId) {
		case EGXColorChannelId::Color0:
			colorDestination = "oColor[0]";
			compDestination = ".rgb";
			break;
		case EGXColorChannelId::Color1:
			colorDestination = "oColor[1]";
			compDestination = ".rgb";
			break;
		case EGXColorChannelId::Alpha0:
			colorDestination = "oColor[0]";
			compDestination = ".a";
			break;
		case EGXColorChannelId::Alpha1:
			colorDestination = "oColor[1]";
			compDestination = ".a";
			break;
	}

	std::string materialSource = colorChannel.MaterialSource == EGXColorSource::Vertex ? "aCol" + std::to_string(index) : "uMaterialReg[" + std::to_string(index) + "]";
	std::string ambientSource = colorChannel.AmbientSource == EGXColorSource::Vertex ? "aCol" + std::to_string(index) : "uAmbientReg[" + std::to_string(index) + "]";

	if (colorChannel.LightingEnabled == false || colorChannel.LightMask == 255) {
		shaderTxt << "\t" << colorDestination << " = " << materialSource << ";\n\n";
		return;
	}

	shaderTxt << "\tlightAccumulator = " << ambientSource << ";\n\n";

	std::string diffuseFunction = "";
	switch (colorChannel.DiffuseFunction) {
		case EGXDiffuseFunction::None:
			diffuseFunction = "1.0";
			break;
		case EGXDiffuseFunction::Signed:
			diffuseFunction = "dot(skinnedNormal, lightDeltaDir)";
			break;
		case EGXDiffuseFunction::Clamp:
			diffuseFunction = "max(dot(skinnedNormal, lightDeltaDir), 0.0)";
			break;
	}

	for (int i = 0; i < 8; i++) {
		if (!(colorChannel.LightMask & (1 << i)))
			continue;

		std::string lightName = "Lights[" + std::to_string(i) + "]";

		std::string attenuationFunction = "";
		switch (colorChannel.AttenuationFunction) {
		case EGXAttenuationFunction::Spec:
		{
			std::string attn = "(dot(skinnedNormal, lightDeltaDir) >= 0.0) ? max(0.0, dot(skinnedNormal, " + lightName + ".Direction.xyz)) : 0.0";
			std::string cosAttn = "ApplyAttenuation(" + lightName + ".AngleAtten.xyz, attenuation)";
			std::string distAttn = "ApplyAttenuation(" + lightName + ".DistAtten.xyz, attenuation)";

			attenuationFunction = "attenuation = " + attn + ";\n\tattenuation = " + cosAttn + " / " + distAttn + ";";
			break;
		}
		case EGXAttenuationFunction::Spot:
		{
			std::string attn = "max(0.0, dot(lightDeltaDir, " + lightName + ".Direction.xyz))";
			std::string cosAttn = "max(0.0, ApplyAttenuation(" + lightName + ".AngleAtten.xyz, " + attn + "))";
			std::string distAtten = "dot(" + lightName + ".DistAtten.xyz, vec3(1.0, lightDeltaDist, lightDeltaDist2))";

			attenuationFunction = "attenuation = " + cosAttn + " / " + distAtten;
			break;
		}
		case EGXAttenuationFunction::None:
			attenuationFunction = "attenuation = 1.0;";
			break;
		}

		shaderTxt << "\t// Light " << i << "\n";
		shaderTxt << "\tlightDelta = " << lightName << ".Position.xyz - aPos.xyz;\n";
		shaderTxt << "\tlightDeltaDist2 = dot(lightDelta, lightDelta);\n";
		shaderTxt << "\tlightDeltaDist = sqrt(lightDeltaDist2);\n";
		shaderTxt << "\tlightDeltaDir = lightDelta / lightDeltaDist;\n\n";
		shaderTxt << "\t" << attenuationFunction << "\n\n";
		shaderTxt << "\tlightAccumulator += " << diffuseFunction << " * attenuation * " << lightName << ".Color;\n\n";
	}

	shaderTxt << "\t// Apply color channel " << index << "\n";
	shaderTxt << "\t" << colorDestination << compDestination << " = (" << materialSource << " * clamp(lightAccumulator, 0.0, 1.0))" << compDestination << ";\n\n";
}
