#include "J3D/J3DShape.hpp"
#include "J3D//J3DShapeFactory.hpp"
#include "GX/GXEnum.hpp"
#include <bstream.h>

void J3DShape::EnableAttributes(std::vector<J3DVCDData>& gxAttributes) {
	for (auto attr : gxAttributes) {
		switch (attr.Attribute) {
			case EGXAttribute::PositionMatrixIdx:
				mEnabledAttributes.push_back(EGLAttribute::SkinWeight);
				mEnabledAttributes.push_back(EGLAttribute::JointID);
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

void J3DShape::Deserialize(bStream::CStream* stream) {
}
