#include "J3D/J3DVertexData.hpp"

J3DVertexGL J3DVertexData::CreateGLVertFromGXVert(const J3DVertexGX& gxVert) {
	J3DVertexGL newGLVert;

	if (HasPositionData())
		newGLVert.Position = Positions[gxVert.Position];
	if (HasNormalData())
		newGLVert.Normal = Normals[gxVert.Normal];

	for (int i = 0; i < 2; i++) {
		if (HasColorData(i))
			newGLVert.Color[i] = Colors[i][gxVert.Color[i]];
	}

	for (int i = 0; i < 8; i++) {
		if (HasTexCoordData(i)) {
			uint16_t index = gxVert.TexCoord[i];
			if (index >= TexCoords[i].size())
				continue;

			newGLVert.TexCoord[i] = TexCoords[i][index];
		}
	}

	return newGLVert;
}
