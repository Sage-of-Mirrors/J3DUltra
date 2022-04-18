#include "J3D/J3DShape.hpp"
#include "J3D/J3DShapeFactory.hpp"

#include <GXGeometryEnums.hpp>
#include <bstream.h>
#include <glad/glad.h>

void J3DShape::EnableAttributes(std::vector<J3DVCDData>& gxAttributes) {
	for (auto attr : gxAttributes) {
		switch (attr.Attribute) {
			case EGXAttribute::PositionMatrixIdx:
				mEnabledAttributes.push_back(EGLAttribute::PositionMatrixIdx);
				break;
			case EGXAttribute::Position:
				mEnabledAttributes.push_back(EGLAttribute::Position);
				break;
			case EGXAttribute::Normal:
				mEnabledAttributes.push_back(EGLAttribute::Normal);
				break;
			case EGXAttribute::Color0:
				mEnabledAttributes.push_back(EGLAttribute::Color0);
				break;
			case EGXAttribute::Color1:
				mEnabledAttributes.push_back(EGLAttribute::Color1);
				break;
			case EGXAttribute::TexCoord0:
				mEnabledAttributes.push_back(EGLAttribute::TexCoord0);
				break;
			case EGXAttribute::TexCoord1:
				mEnabledAttributes.push_back(EGLAttribute::TexCoord1);
				break;
			case EGXAttribute::TexCoord2:
				mEnabledAttributes.push_back(EGLAttribute::TexCoord2);
				break;
			case EGXAttribute::TexCoord3:
				mEnabledAttributes.push_back(EGLAttribute::TexCoord3);
				break;
			case EGXAttribute::TexCoord4:
				mEnabledAttributes.push_back(EGLAttribute::TexCoord4);
				break;
			case EGXAttribute::TexCoord5:
				mEnabledAttributes.push_back(EGLAttribute::TexCoord5);
				break;
			case EGXAttribute::TexCoord6:
				mEnabledAttributes.push_back(EGLAttribute::TexCoord6);
				break;
			case EGXAttribute::TexCoord7:
				mEnabledAttributes.push_back(EGLAttribute::TexCoord7);
				break;
		}
	}
}

bool J3DShape::HasEnabledAttribute(const EGLAttribute attribute) const {
	for (auto a : mEnabledAttributes) {
		if (a == attribute)
			return true;
	}

	return false;
}

void J3DShape::ConcatenatePacketsToIBO(std::vector<J3DVertexGX>* ibo) {
	if (ibo == nullptr)
		return;

	mIBOStart = ibo->size();

	for (auto a : mPackets) {
		mIBOCount += a.mVertices.size();

		for (auto t : a.mVertices)
			ibo->push_back(t);
	}
}

void J3DShape::RenderShape() {
	glDrawElements(GL_TRIANGLES, mIBOCount, GL_UNSIGNED_SHORT, (const void*)(mIBOStart * sizeof(uint16_t)));
}

void J3DShape::Deserialize(bStream::CStream* stream) {
}
