#include "J3D/J3DUtil.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

std::string J3DUtility::LoadTextFile(std::filesystem::path filePath) {
	if (filePath.empty() || !std::filesystem::exists(filePath))
		return "";

	std::fstream shaderFile(filePath);
	if (!shaderFile.is_open()) {
		std::cout << "Unable to load text from " << filePath << std::endl;
		return "";
	}

	std::stringstream iss;
	iss << shaderFile.rdbuf();
	std::string shaderTxt = iss.str();

	shaderFile.close();

	return shaderTxt;
}
