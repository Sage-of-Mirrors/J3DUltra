#include "J3D/J3DVertexShaderGenerator.hpp"
#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DShape.hpp"
#include "J3D/J3DUtil.hpp"

#include <glad/glad.h>

// For debugging
#include <iostream>
#include <fstream>

bool J3DVertexShaderGenerator::GenerateVertexShader(const J3DMaterial* material, const int32_t& jointCount, uint32_t& shaderHandle) {
	if (material == nullptr || material->GetShape() == nullptr)
		return false;
	
	shaderHandle = glCreateShader(GL_VERTEX_SHADER);

	/*
	std::stringstream shaderStream;
	shaderStream << "#version 330 core\n\n";

	WriteAttributes(shaderStream, material->GetShape()->GetEnabledAttributes());
	WriteOutputs(shaderStream, material);
	//WriteUniforms(shaderStream, jointCount);
	WriteMainFunction(shaderStream, material);

	shaderHandle = glCreateShader(GL_VERTEX_SHADER);
	
	std::string shaderStr = shaderStream.str();

	std::ofstream debugVOut("./debugvtxshader.glsl");
	if (debugVOut.is_open()) {
		debugVOut << shaderStr;
		debugVOut.close();
	}*/

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
				shaderTxt << "vec2 aTex" << (uint32_t)a - (uint32_t)EGLAttribute::TexCoord0 << ";\n";
				break;
		}
	}

	shaderTxt << "\n";
}

void J3DVertexShaderGenerator::WriteOutputs(std::stringstream& shaderTxt, const J3DMaterial* material) {
	shaderTxt << "// Vertex shader outputs\n";

	//shaderTxt << "// Number of color channel controls: " << material->ChannelControlCount << "\n";
	shaderTxt << "out vec4 oColor0;\n"
		"out vec4 oColor1;\n\n";

	// TODO: Tex coord gens
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
			"\tmat4 Joints[" << jointCount << "];\n"
		"};\n\n";

	shaderTxt << "// These uniforms can be modified per-material, usually by external animations.\n";
	shaderTxt << "uniform vec4 uColor0_Amb;\n"
		"uniform vec4 uColor0_Mat;\n"
		"uniform vec4 uColor1_Amb;\n"
		"uniform vec4 uColor1_Mat;\n\n";
}

void J3DVertexShaderGenerator::WriteSkinningFunction(std::stringstream& shaderTxt) {
	shaderTxt << "vec4 applySkin(in vec3 pos) {\n"
			"\tvec4 skinnedPos = vec4(pos, 1.0f);\n\n"
			"\tfor (int i = 0; i > 4; i++) {\n"
			"\t\tskinnedPos = (Joints[aJointIDs[i]] * aSkinWeights[i]) * skinnedPos;\n"
			"\t}\n\n"
			"\treturn skinnedPos;\n"
		"}\n";
}

void J3DVertexShaderGenerator::WriteMainFunction(std::stringstream& shaderTxt, const J3DMaterial* material) {
	shaderTxt << "// Main shader program\n";

	// Start of main function
	shaderTxt << "void main() {\n";

	// Position calculations
	shaderTxt << //"\tmat4 MVP = Proj * View * Model;\n"
		//"\tvec4 skinnedPos = applySkin(aPos);\n"
		"\tgl_Position = aPos;\n"
		"\toColor0";
		//"\tvec4 worldPos = Model * skinnedPos;\n\n";

	// End of main function
	shaderTxt << "}\n\n";
}
