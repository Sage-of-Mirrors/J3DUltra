#include "J3D/J3DVertexData.hpp"

J3DVertexGL J3DVertexData::CreateGLVertFromGXVert(const J3DVertexGX& gxVert) {
	J3DVertexGL newGLVert;

	if (HasPositionData() && gxVert.Position < Positions.size())
		newGLVert.Position = Positions[gxVert.Position];
	if (HasNormalData() && gxVert.Normal < Normals.size())
		newGLVert.Normal = Normals[gxVert.Normal];

	for (int i = 0; i < 2; i++) {
		if (HasColorData(i) && gxVert.Color[i] < Colors[i].size())
			newGLVert.Color[i] = Colors[i][gxVert.Color[i]];
	}

	for (int i = 0; i < 8; i++) {
		if (HasTexCoordData(i) && gxVert.TexCoord[i] < TexCoords[i].size()) {
			newGLVert.TexCoord[i] = TexCoords[i][gxVert.TexCoord[i]];
		}
	}

	return newGLVert;
}
