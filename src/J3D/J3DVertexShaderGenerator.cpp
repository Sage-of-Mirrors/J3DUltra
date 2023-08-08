#include "J3D/J3DVertexShaderGenerator.hpp"
#include "J3D/J3DMaterialData.hpp"
#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DShape.hpp"
#include "J3D/J3DUtil.hpp"
#include "J3D/J3DShaderGeneratorCommon.hpp"

#include "GXGeometryData.hpp"
#include <magic_enum.hpp>
#include <glad/gl.h>
#include <sstream>

// For debugging
#include <iostream>
#include <fstream>

#define etoi magic_enum::enum_integer

bool J3DVertexShaderGenerator::GenerateVertexShader(const J3DMaterial* material, uint32_t& shaderHandle) {
	if (material == nullptr || material->GetShape() == nullptr)
		return false;
	
	shaderHandle = glCreateShader(GL_VERTEX_SHADER);

	std::stringstream vertexShader;
	vertexShader << GenerateAttributes(material->GetShape()->GetAttributeTable());
	vertexShader << GenerateOutputs(material);
	vertexShader << GenerateUniforms();

	vertexShader << "float ApplyAttenuation(vec3 t_Coeff, float t_Value) {\n"
		"\treturn dot(t_Coeff, vec3(1.0, t_Value, t_Value * t_Value));\n"
		"}\n\n";

	// Convert the given float to an int in the range 0..255.
	vertexShader << "int FloatToS10(float f) {\n";
	vertexShader << "\treturn int(f * 255.0) & 0xFF;\n";
	vertexShader << "}\n\n";

	// Convert a float vec4 with components in range 0..1 to an integer vec4 with components 0..255.
	vertexShader << "ivec4 VecFloatToS10(vec4 a) {\n";
	vertexShader << "\treturn ivec4(FloatToS10(a.r), FloatToS10(a.g), FloatToS10(a.b), FloatToS10(a.a));\n";
	vertexShader << "}\n\n";

	// Convert a float vec3 with components in range 0..1 to an integer vec4 with components 0..255.
	vertexShader << "ivec3 VecFloatToS10(vec3 a) {\n";
	vertexShader << "\treturn ivec3(FloatToS10(a.r), FloatToS10(a.g), FloatToS10(a.b));\n";
	vertexShader << "}\n\n";

	bool hasNormals = J3DUtility::VectorContains(material->GetShape()->GetAttributeTable(), EGXAttribute::Normal);
	vertexShader << GenerateMainFunction(material, hasNormals);

	shaderHandle = glCreateShader(GL_VERTEX_SHADER);
	
	std::string shaderStr = vertexShader.str();

	std::ofstream debugVOut("./shader/" + material->Name + "_vtx.glsl");
	if (debugVOut.is_open()) {
		debugVOut << shaderStr;
		debugVOut.close();
	}

	//std::string shaderChars = J3DUtility::LoadTextFile("./res/shaders/Debug_NormalColors.vert");
	const char* s = shaderStr.c_str();

	glShaderSource(shaderHandle, 1, &s, NULL);
	glCompileShader(shaderHandle);

	int32_t success = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::cout << "Vertex shader compilation for " << material->Name << " failed! Details:" << std::endl;

		int32_t logSize = 0;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logSize);

		std::vector<char> log(logSize);
		glGetShaderInfoLog(shaderHandle, logSize, nullptr, &log[0]);

		std::cout << std::string(log.data()) << std::endl;

		glDeleteShader(shaderHandle);

		return false;
	}

	return true;
}

std::string J3DVertexShaderGenerator::GenerateAttributes(const std::vector<EGXAttribute>& shapeAttributes) {
	std::stringstream stream;
	stream << "#version 330\n\n";
	stream << "// Input attributes\n";

	for (auto a : shapeAttributes) {
		if (a == EGXAttribute::PositionMatrixIdx || a == EGXAttribute::Tex0MatrixIdx || a == EGXAttribute::Tex1MatrixIdx
			|| a == EGXAttribute::Tex2MatrixIdx || a == EGXAttribute::Tex3MatrixIdx || a == EGXAttribute::Tex4MatrixIdx
			|| a == EGXAttribute::Tex5MatrixIdx || a == EGXAttribute::Tex6MatrixIdx || a == EGXAttribute::Tex7MatrixIdx)
		{
			continue;
		}

		stream << "layout (location = " << (uint32_t)a << ") in ";

		switch (a) {
			case EGXAttribute::Position:
				stream << "vec4 aPos;\n";
				break;
			case EGXAttribute::Normal:
				stream << "vec3 aNrm;\n";
				break;
			case EGXAttribute::Color0:
			case EGXAttribute::Color1:
				stream << "vec4 aCol" << etoi(a) - etoi(EGXAttribute::Color0)<< ";\n";
				break;
			case EGXAttribute::TexCoord0:
			case EGXAttribute::TexCoord1:
			case EGXAttribute::TexCoord2:
			case EGXAttribute::TexCoord3:
			case EGXAttribute::TexCoord4:
			case EGXAttribute::TexCoord5:
			case EGXAttribute::TexCoord6:
			case EGXAttribute::TexCoord7:
				stream << "vec3 aTex" << etoi(a) - etoi(EGXAttribute::TexCoord0) << ";\n";
				break;
			default:
				//stream << "int aUnk;\n";
				break;
		}
	}

	stream << "\n";
	return stream.str();
}

std::string J3DVertexShaderGenerator::GenerateOutputs(const J3DMaterial* material) {
	std::stringstream stream;
	stream << "// Vertex shader outputs\n";

	stream << "// Number of color channel controls: " << std::to_string(material->LightBlock.mColorChannels.size()) << "\n";
	stream << "out vec4 oColor0;\n";
	stream << "out vec4 oColor1;\n\n";

	uint32_t texGenCount = material->TexGenBlock.mTexCoordInfo.size();
	stream << "// Tex gen count: " << texGenCount << "\n";
	for (int i = 0; i < texGenCount; i++) {
		stream << "out vec3 oTexCoord" << i << ";\n";
	}

	stream << "\n";
	return stream.str();
}

std::string J3DVertexShaderGenerator::GenerateUniforms() {
	std::stringstream stream;
	stream << "// These uniforms can be modified per-material, usually by external animations.\n";

	stream << "uniform vec4 uMaterialReg[2];\n";
	stream << "uniform vec4 uAmbientReg[2];\n\n";

	stream << J3DShaderGeneratorCommon::GenerateStructs();

	return stream.str();
}

std::string J3DVertexShaderGenerator::GenerateTexGen(std::shared_ptr<J3DTexCoordInfo> texGen, const uint32_t index) {
	std::stringstream stream;
	stream << "\t// TexGen Source: " << magic_enum::enum_name(texGen->Source) << ", ";
	stream << "Type: " << magic_enum::enum_name(texGen->Type) << ", ";
	stream << "TexMatrix: " << magic_enum::enum_name(texGen->TexMatrix) << "\n";

	stream << "\toTexCoord" << index << " = ";

	// Figure out where the initial tex coord is coming from.
	std::stringstream source;
	switch (texGen->Source) {
		case EGXTexGenSrc::Position:
			source << "vec4(aPos.xyz, 1.0)";
			break;
		case EGXTexGenSrc::Normal:
			source << "vec4(aNrm.xyz, 1.0)";
			break;
		case EGXTexGenSrc::Binormal:
			source << "vec4(aBin.xyz, 1.0)";
			break;
		case EGXTexGenSrc::Tangent:
			source << "vec4(aTan.xyz, 1.0)";
			break;
		case EGXTexGenSrc::Color0:
			source << "oColor0";
			break;
		case EGXTexGenSrc::Color1:
			source << "oColor1";
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
			uint32_t texIndex = etoi(texGen->Source) - etoi(EGXTexGenSrc::Tex0);
			source << "vec4(aTex" << texIndex << ".xy, 1.0, 1.0)";
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
			uint32_t texCoordIndex = etoi(texGen->Source) - etoi(EGXTexGenSrc::TexCoord0);
			source << "vec4(oTexCoord" << texCoordIndex << ".xyz, 1.0)";
			break;
		}
	}

	// Apply a texmatrix to the coords we found above.
	if (texGen->TexMatrix == EGXTexMatrix::Identity) {
		if (texGen->Type == EGXTexGenType::SRTG)
			stream << "vec3(" << source.str() << ".xy, 1.0);\n";
		else
			stream << source.str() << ".xyz;\n";
	}
	else {
		uint32_t texMatrixIndex = (etoi(texGen->TexMatrix) - etoi(EGXTexMatrix::TexMtx0)) / 3;

		switch (texGen->Type) {
			case EGXTexGenType::Matrix2x4:
				stream << "vec3((TexMatrices[" << texMatrixIndex << "] * " << source.str() << ").xy, 1.0);\n";
				break;
			case EGXTexGenType::Matrix3x4:
			{
				stream << "(TexMatrices[" << texMatrixIndex << "] * " << source.str() << ").xyz;\n";
				break;
			}
			case EGXTexGenType::SRTG:
				stream << "vec3(" << source.str() << ".rg, 1.0);\n";
				break;
			default:
				stream << source.str() << ".xyz;\n";
				break;
			}
	}

	return stream.str();
}

std::string J3DVertexShaderGenerator::GenerateLight(std::shared_ptr<J3DColorChannel> colorChannel, const uint32_t& lightIndex) {
	std::stringstream stream;

	std::string lightName = "Lights[" + std::to_string(lightIndex) + "]";
	std::string diffuseFunction = "";
	std::string attenuationFunction = "";

	// Generate diffuse function
	switch (colorChannel->DiffuseFunction) {
		case EGXDiffuseFunction::Signed:
			diffuseFunction = "dot(SkinnedNormal, PosLightDir)";
			break;
		case EGXDiffuseFunction::Clamp:
			diffuseFunction = "max(dot(SkinnedNormal, PosLightDir), 0.0)";
			break;
		case EGXDiffuseFunction::None:
		default:
			diffuseFunction = "1.0";
			break;
	}

	std::string attn = "1.0";
	// Generate attenuation function
	switch (colorChannel->AttenuationFunction) {
		case EGXAttenuationFunction::Spec:
		{
			attn = "(dot(SkinnedNormal, PosLightDir) >= 0.0) ? max(0.0, dot(SkinnedNormal, " + lightName + ".Direction.xyz)) : 0.0";
			std::string cosAttn = "ApplyAttenuation(" + lightName + ".AngleAtten.xyz, Attenuation)";
			std::string distAttn = "ApplyAttenuation(" + lightName + ".DistAtten.xyz, Attenuation)";

			attenuationFunction = "Attenuation = " + attn + ";\n\tAttenuation = " + cosAttn + " / " + distAttn + ";";
			break;
		}
		case EGXAttenuationFunction::Spot:
		{
			attn = "max(0.0, dot(PosLightDir, " + lightName + ".Direction.xyz))";
			std::string cosAttn = "max(0.0, ApplyAttenuation(" + lightName + ".AngleAtten.xyz, " + attn + "))";
			std::string distAtten = "dot(" + lightName + ".DistAtten.xyz, vec3(1.0, PosLightDist, PosLightDistSq))";

			attenuationFunction = "Attenuation = " + cosAttn + " / " + distAtten + ";";
			break;
		}
		case EGXAttenuationFunction::None:
		default:
			attenuationFunction = "Attenuation = 1.0;";
			break;
	}

	stream << "\t\t// Light " << lightIndex << "\n";
	stream << "\t\tPosLightVec = " << lightName << ".Position.xyz - WorldPos.xyz;\n";
	stream << "\t\tPosLightDistSq = dot(PosLightVec, PosLightVec);\n";
	stream << "\t\tPosLightDist = sqrt(PosLightDistSq);\n";
	stream << "\t\tPosLightDir = PosLightVec / PosLightDist;\n\n";
	stream << "\t\tAttenuation = " << attn << ";\n";
	stream << "\t\t" << attenuationFunction << "\n\n";
	stream << "\t\tAccumulator += " << diffuseFunction << " * Attenuation * " << lightName << ".Color;\n\n";

	return stream.str();
}

std::string J3DVertexShaderGenerator::GenerateColorChannel(std::shared_ptr<J3DColorChannel> colorChannel, const int32_t& index) {
	std::stringstream stream;

	stream << "\t// Color channel " << index << ": " <<
		"Lighting enabled: " << std::to_string(colorChannel->LightingEnabled) << ", " <<
		"Material Source: " << magic_enum::enum_name(colorChannel->MaterialSource) << ", " <<
		"Light Mask: " << std::to_string(colorChannel->LightMask) << ", " <<
		"Diffuse Function: " << magic_enum::enum_name(colorChannel->DiffuseFunction) << ", " <<
		"Atten Function: " << magic_enum::enum_name(colorChannel->AttenuationFunction) << ", " <<
		"Ambient Source: " << magic_enum::enum_name(colorChannel->AmbientSource) << "\n";

	stream << "\t{\n";

	EGXColorChannelId chanId = magic_enum::enum_value<EGXColorChannelId>(index);
	std::string colorDestination = "";
	std::string compDestination = "";
	uint32_t channelIndex = 0;

	switch (chanId) {
	case EGXColorChannelId::Color0:
		colorDestination = "oColor0";
		compDestination = ".rgb";
		break;
	case EGXColorChannelId::Alpha0:
		colorDestination = "oColor0";
		compDestination = ".a";
		break;
	case EGXColorChannelId::Color1:
		colorDestination = "oColor1";
		compDestination = ".rgb";
		channelIndex = 1;
		break;
	case EGXColorChannelId::Alpha1:
		colorDestination = "oColor1";
		compDestination = ".a";
		channelIndex = 1;
		break;
	}

	std::string materialSource = colorChannel->MaterialSource == EGXColorSource::Vertex ? "aCol" + std::to_string(channelIndex) : "uMaterialReg[" + std::to_string(channelIndex) + "]";
	std::string ambientSource = colorChannel->AmbientSource == EGXColorSource::Vertex ? "aCol" + std::to_string(channelIndex) : "uAmbientReg[" + std::to_string(channelIndex) + "]";

	if (colorChannel->LightingEnabled == false) {
		stream << "\t\t" << colorDestination << compDestination << " = " << materialSource << compDestination << ";\n";
		stream << "\t}\n\n";
		return stream.str();
	}

	stream << "\t\tvec4 Accumulator = " << ambientSource << ";\n\n";
	stream << "\t\tvec3 PosLightVec, PosLightDir;\n";
	stream << "\t\tfloat PosLightDistSq, PosLightDist, Attenuation;\n\n";

	for (int i = 0; i < 8; i++) {
		if (!(colorChannel->LightMask & (1 << i)))
			continue;

		stream << GenerateLight(colorChannel, i);
	}

	stream << "\t\t// Apply color channel " << index << "\n";

	if (chanId == EGXColorChannelId::Color0 || chanId == EGXColorChannelId::Alpha0)
		stream << "\t\t" << colorDestination << compDestination << " = ((" << materialSource << " * clamp(Accumulator, 0.0, 1.0))" << compDestination << ");\n";
	else if (chanId == EGXColorChannelId::Color1 || chanId == EGXColorChannelId::Alpha1)
		stream << "\t\t" << colorDestination << compDestination << " = ((" << materialSource << " * clamp(Accumulator, 0.0, 1.0))" << compDestination << ");\n";

	stream << "\t}\n\n";
	return stream.str();
}

std::string J3DVertexShaderGenerator::GenerateMainFunction(const J3DMaterial* material, const bool hasNormals) {
	std::stringstream stream;
	stream << "void main() {\n";

	if (hasNormals)
		stream << "\tvec3 SkinnedNormal = mat3(transpose(inverse(Envelopes[int(aPos.w)]))) * aNrm;\n";
	stream << "\tmat4 MVP = Proj * View * Model;\n";
	stream << "\tvec4 WorldPos = MVP * (Envelopes[int(aPos.w)]) * vec4(aPos.xyz, 1);\n\n";

	bool wroteAlpha0 = false;
	bool wroteAlpha1 = false;
	for (int i = 0; i < material->LightBlock.mColorChannels.size(); i++) {
		stream << GenerateColorChannel(material->LightBlock.mColorChannels[i], i);

		if (magic_enum::enum_value<EGXColorChannelId>(i) == EGXColorChannelId::Alpha0)
			wroteAlpha0 = true;
		if (magic_enum::enum_value<EGXColorChannelId>(i) == EGXColorChannelId::Alpha1)
			wroteAlpha1 = true;
	}

	if (!wroteAlpha0) {
		stream << "\toColor0.a = 1.0;\n";
	}
	if (!wroteAlpha1) {
		stream << "\toColor1.a = 1.0;\n";
	}

	stream << "\n";

	for (int i = 0; i < material->TexGenBlock.mTexCoordInfo.size(); i++)
		stream << GenerateTexGen(material->TexGenBlock.mTexCoordInfo[i], i);

	// End of main function
	stream << "\n\tgl_Position = WorldPos;\n";
	stream << "}\n";
	return stream.str();
}
