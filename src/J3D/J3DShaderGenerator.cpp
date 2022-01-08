#include "J3D/J3DShaderGenerator.hpp"
#include "J3D/J3DShape.hpp"

#include <glad/glad.h>

// For debugging
#include <iostream>
#include <fstream>

std::string J3DShaderGenerator::LoadDebugShader(std::string filename) {
	std::string shaderPath = "./res/shaders/" + filename;

	std::fstream shaderFile(shaderPath);
	if (!shaderFile.is_open()) {
		std::cout << "Unable to load debug shader from " << shaderPath << std::endl;
		return "";
	}

	std::stringstream iss;
	iss << shaderFile.rdbuf();
	std::string shaderTxt = iss.str();

	shaderFile.close();

	return shaderTxt;
}

bool J3DShaderGenerator::GenerateVertexShader(const std::vector<EGLAttribute>& shapeAttributes, int32_t& shaderHandle) {
	std::stringstream shaderStream;
	shaderStream << "#version 330 core\n\n";

	WriteAttributes(shaderStream, shapeAttributes);

	shaderHandle = glCreateShader(GL_VERTEX_SHADER);
	
	std::string shaderStr = shaderStream.str();
	const char* shaderChars = shaderStr.c_str();
	glShaderSource(shaderHandle, 1, &shaderChars, NULL);

	glCompileShader(shaderHandle);

	int32_t success = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::cout << "Vertex shader compilation failed!" << std::endl;

		return false;
	}

	return true;
}

bool J3DShaderGenerator::GenerateFragmentShader(int32_t& shaderHandle) {
	// TODO: actual fragment shader generation
	std::string shaderStr = LoadDebugShader("debug_normalcolors.frag");
	if (shaderStr.empty())
		return false;

	shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

	const char* shaderChars = shaderStr.c_str();
	glShaderSource(shaderHandle, 1, &shaderChars, NULL);

	glCompileShader(shaderHandle);

	int32_t success = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::cout << "Fragment shader compilation failed!" << std::endl;

		return false;
	}

	return true;
}

void J3DShaderGenerator::WriteAttributes(std::stringstream& shaderTxt, const std::vector<EGLAttribute>& shapeAttributes) {
	for (auto a : shapeAttributes) {
		shaderTxt << "layout (location = " << (uint32_t)a << ") in ";

		switch (a) {
			case EGLAttribute::Position:
				shaderTxt << "vec3 aPos;\n";
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
				shaderTxt << "vec4 aTex" << (uint32_t)a - (uint32_t)EGLAttribute::TexCoord0 << ";\n";
				break;
			// SkinWeight implies JointID
			case EGLAttribute::SkinWeight:
				shaderTxt << "vec4 aSkinWeights;\n";
				shaderTxt << "vec4 aJointIDs;\n";
				break;
		}
	}

	shaderTxt << "\n";
}

void J3DShaderGenerator::WriteSkinningFunction(std::stringstream& shaderTxt) {
	shaderTxt << "vec4 applySkin(in vec3 pos) {\n"
		"\tvec4 skinnedPos = vec4(pos, 1.0f);\n\n"
		"\tfor (int i = 0; i > 4; i++) {\n"
		"\t\tskinnedPos = skinnedPos * (Joints[JointIndices[i]] * SkinWeights[i]);\n"
		"\t}\n"
		"\treturn skinnedPos;\n"
		"}\n";
}
