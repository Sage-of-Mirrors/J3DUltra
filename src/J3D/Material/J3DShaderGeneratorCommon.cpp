#include "J3D/Material/J3DShaderGeneratorCommon.hpp"

#include <sstream>


std::string J3DShaderGeneratorCommon::GenerateStructs() {
	std::stringstream stream;

	stream << "// Represents a hardware light source.\n";
	stream << "struct GXLight {\n"
		"\tvec4 Position;\n"
		"\tvec4 Direction;\n"
		"\tvec4 Color;\n"
		"\tvec4 AngleAtten;\n"
		"\tvec4 DistAtten;\n"
		"};\n\n";

	stream << "// This UBO contains data that doesn't change between vertices or materials.\n";
	stream << "layout (std140, binding=0) uniform uSharedData {\n"
		"\tmat4 Proj;\n"
		"\tmat4 View;\n"
		"\tmat4 Model;\n\n"
		"\tvec4 TevColor[4];\n"
		"\tvec4 KonstColor[4];\n\n"
		"\tGXLight Lights[8];\n"
		"\tmat4 Envelopes[512];\n"
		"\tmat4 TexMatrices[10];\n"
		"\tuint BillboardType;\n"
		"\tuint ModelId;\n"
		"\tuint MaterialId;\n"
		"\tvec4 HighlightColor;\n"
		"};\n\n";

	return stream.str();
}
