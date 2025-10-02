#include "J3D/Material/J3DVertexShaderGenerator.hpp"
#include "J3D/Material/J3DMaterialData.hpp"
#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Material/J3DShaderGeneratorCommon.hpp"
#include "J3D/Geometry/J3DShape.hpp"
#include "J3D/Util/J3DUtil.hpp"

#include "GXGeometryData.hpp"
#include <magic_enum.hpp>
#include <glad/glad.h>
#include <sstream>
#include <algorithm>

// For debugging
#include <iostream>
#include <fstream>

#define etoi magic_enum::enum_integer

bool J3DVertexShaderGenerator::GenerateVertexShader(const J3DMaterial* material, uint32_t& shaderHandle) {
	if (material == nullptr || material->GetShape().expired()) {
		return false;
	}
	
	shaderHandle = glCreateShader(GL_VERTEX_SHADER);

	std::stringstream vertexShader;
	vertexShader << GenerateAttributes(material);
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

	vertexShader << GenerateMatrixCalcFunction();

	bool hasNormals = J3DUtility::VectorContains(material->GetShape().lock()->GetAttributeTable(), EGXAttribute::Normal);
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

bool J3DVertexShaderGenerator::IsAttributeUsed(EGXAttribute a, const J3DMaterial* material) {
	switch (a) {
		case EGXAttribute::Position:
		{
			return true;
		}
		case EGXAttribute::Normal:
		{
			bool colorChansUseNormal = false;

			for (auto c : material->LightBlock.mColorChannels) {
				if (!c->LightingEnabled || c->LightMask == 0) {
					continue;
				}

				if (c->DiffuseFunction != EGXDiffuseFunction::None || c->AttenuationFunction == EGXAttenuationFunction::Spec) {
					colorChansUseNormal = true;
					break;
				}
			}

			auto texCoordItr = std::find_if(
				material->TexGenBlock.mTexCoordInfo.begin(),
				material->TexGenBlock.mTexCoordInfo.end(),
				[](const std::shared_ptr<J3DTexCoordInfo>& a) { return a->Source == EGXTexGenSrc::Normal; }
			);

			return colorChansUseNormal || texCoordItr != material->TexGenBlock.mTexCoordInfo.end();
		}
		case EGXAttribute::Color0:
		case EGXAttribute::Color1:
		{
			uint32_t colorIndex = (static_cast<uint32_t>(a) - static_cast<uint32_t>(EGXAttribute::Color0)) * 2;

			if (material->LightBlock.mColorChannels.size() < colorIndex || material->LightBlock.mColorChannels.size() < colorIndex + 1) {
				return false;
			}

			if (material->LightBlock.mColorChannels[colorIndex]->MaterialSource == EGXColorSource::Vertex || // Color channel
				material->LightBlock.mColorChannels[colorIndex]->AmbientSource == EGXColorSource::Vertex ||
				material->LightBlock.mColorChannels[colorIndex + 1]->MaterialSource == EGXColorSource::Vertex || // Alpha channel
				material->LightBlock.mColorChannels[colorIndex + 1]->AmbientSource == EGXColorSource::Vertex) {
				return true;
			}

			return false;
		}
		case EGXAttribute::TexCoord0:
		case EGXAttribute::TexCoord1:
		case EGXAttribute::TexCoord2:
		case EGXAttribute::TexCoord3:
		case EGXAttribute::TexCoord4:
		case EGXAttribute::TexCoord5:
		case EGXAttribute::TexCoord6:
		case EGXAttribute::TexCoord7:
		{
			EGXTexGenSrc curSrc = static_cast<EGXTexGenSrc>(
				(static_cast<int>(a) - static_cast<int>(EGXAttribute::TexCoord0)) +
				static_cast<int>(EGXTexGenSrc::Tex0)
			);

			auto itr = std::find_if(
				material->TexGenBlock.mTexCoordInfo.begin(),
				material->TexGenBlock.mTexCoordInfo.end(),
				[&curSrc](const std::shared_ptr<J3DTexCoordInfo>& a) { return a->Source == curSrc; }
			);

			return itr != material->TexGenBlock.mTexCoordInfo.end();
		}
		default:
		{
			return false;
		}
	}
}

std::string J3DVertexShaderGenerator::GenerateAttributes(const J3DMaterial* material) {
	std::stringstream stream;
	stream << "#version 460\n\n";
	stream << "// Input attributes\n";

	for (uint32_t i = 0; i < static_cast<uint32_t>(EGXAttribute::Attribute_Max); i++) {
		EGXAttribute a = static_cast<EGXAttribute>(i);

		if (IsAttributeUsed(a, material)) {
			stream << "layout (location = " << static_cast<uint32_t>(a) << ") in ";

			switch (a) {
			case EGXAttribute::Position:
				stream << "vec4 aPos;\n";
				break;
			case EGXAttribute::Normal:
				stream << "vec3 aNrm;\n";
				break;
			case EGXAttribute::Color0:
			case EGXAttribute::Color1:
				stream << "vec4 aCol" << etoi(a) - etoi(EGXAttribute::Color0) << ";\n";
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
	uint32_t texMatrixIndex = texGen->TexMatrix == EGXTexMatrix::Identity ?
		index :
		(etoi(texGen->TexMatrix) - etoi(EGXTexMatrix::TexMtx0)) / 3;

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
			diffuseFunction = "dot(ViewNormal, PosLightDir)";
			break;
		case EGXDiffuseFunction::Clamp:
			diffuseFunction = "max(dot(ViewNormal, PosLightDir), 0.0)";
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
			attn = "(dot(ViewNormal, PosLightDir) >= 0.0) ? max(0.0, dot(ViewNormal, (PosLightTransform * " + lightName + ".Direction).xyz)) : 0.0";
			std::string cosAttn = "ApplyAttenuation(" + lightName + ".AngleAtten.xyz, Attenuation)";
			std::string distAttn = "ApplyAttenuation(" + lightName + ".DistAtten.xyz, Attenuation)";

			attenuationFunction = "Attenuation = " + attn + ";\n\tAttenuation = " + cosAttn + " / " + distAttn + ";";
			break;
		}
		case EGXAttenuationFunction::Spot:
		{
			attn = "max(0.0, dot(PosLightDir, (PosLightTransform * " + lightName + ".Direction).xyz))";
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
	stream << "\t\tPosLightTransform = int(" << lightName << ".Position.w) == 1 ? View : mat4(1.0);\n";
	stream << "\t\tPosLightVec = (PosLightTransform * vec4(" << lightName << ".Position.xyz, 1.0)).xyz - ViewPos;\n";
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
	stream << "\t\tmat4 PosLightTransform;\n\n";

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

std::string J3DVertexShaderGenerator::GenerateMatrixCalcFunction() {
	std::stringstream stream;
	stream << "vec3 CalculateMatrix() {\n";
	stream << "\tmat4 envelopeMtx = View * Model * Envelopes[int(aPos.w)];\n\n";

	stream << "\tif (BillboardType == 0 || BillboardType == 3) {\n";
	stream << "\t\treturn (envelopeMtx * vec4(aPos.xyz, 1.0)).xyz;\n";
	stream << "\t}\n\n";

	stream << "\tmat4 bboardMtx = mat4(1.0);\n\n";
	stream << "\tfloat mx = envelopeMtx[0][0] * envelopeMtx[0][0] + envelopeMtx[0][1] * envelopeMtx[0][1] + envelopeMtx[0][2] * envelopeMtx[0][2];\n";
	stream << "\tfloat my = envelopeMtx[1][0] * envelopeMtx[1][0] + envelopeMtx[1][1] * envelopeMtx[1][1] + envelopeMtx[1][2] * envelopeMtx[1][2];\n";
	stream << "\tfloat mz = envelopeMtx[2][0] * envelopeMtx[2][0] + envelopeMtx[2][1] * envelopeMtx[2][1] + envelopeMtx[2][2] * envelopeMtx[2][2];\n";

	stream << "\tif (BillboardType == 1) {\n";
	stream << "\t\tbboardMtx[0][0] = mx;\n";
	stream << "\t\tbboardMtx[0][1] = 0.0;\n";
	stream << "\t\tbboardMtx[0][2] = 0.0;\n\n";

	stream << "\t\tbboardMtx[1][0] = 0.0;\n";
	stream << "\t\tbboardMtx[1][1] = my;\n";
	stream << "\t\tbboardMtx[1][2] = 0.0;\n\n";

	stream << "\t\tbboardMtx[2][0] = 0.0;\n";
	stream << "\t\tbboardMtx[2][1] = 0.0;\n";
	stream << "\t\tbboardMtx[2][2] = mz;\n";
	stream << "\t}\n";

	stream << "\telse {\n";
	stream << "\t\tvec3 vs = normalize(vec3(0.0, -envelopeMtx[1][2], envelopeMtx[1][1]));\n\n";

	stream << "\t\tbboardMtx[0][0] = mx;\n";
	stream << "\t\tbboardMtx[0][1] = 0.0;\n";
	stream << "\t\tbboardMtx[0][2] = 0.0;\n\n";

	stream << "\t\tbboardMtx[1][0] = envelopeMtx[1][0];\n";
	stream << "\t\tbboardMtx[1][1] = envelopeMtx[1][1];\n";
	stream << "\t\tbboardMtx[1][2] = envelopeMtx[1][2];\n\n";

	stream << "\t\tbboardMtx[2][0] = 0.0;\n";
	stream << "\t\tbboardMtx[2][1] = vs.y * mz;\n";
	stream << "\t\tbboardMtx[2][2] = vs.z * mz;\n";
	stream << "\t}\n\n";

	stream << "\tbboardMtx[3][0] = envelopeMtx[3][0];\n";
	stream << "\tbboardMtx[3][1] = envelopeMtx[3][1];\n";
	stream << "\tbboardMtx[3][2] = envelopeMtx[3][2];\n\n";

	stream << "\treturn (bboardMtx * vec4(aPos.xyz, 1.0)).xyz;\n";
	stream << "}\n\n";

	return stream.str();
}

std::string J3DVertexShaderGenerator::GenerateMainFunction(const J3DMaterial* material, const bool hasNormals) {
	std::stringstream stream;
	stream << "void main() {\n";

	stream << "\tvec3 ViewPos = CalculateMatrix();\n";
	if (IsAttributeUsed(EGXAttribute::Normal, material)) {
		stream << "\tvec3 ViewNormal = (View * Model * vec4(mat3(transpose(inverse(Envelopes[int(aPos.w)]))) * aNrm, 0.0)).xyz;\n";
	}

	stream << "\n";

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
	stream << "\n\tgl_Position = Proj * vec4(ViewPos, 1.0);\n";
	stream << "}\n";
	return stream.str();
}
