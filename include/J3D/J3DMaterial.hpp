#pragma once

#include "J3D/J3DMaterialData.hpp"
#include "GX/GXEnum.hpp"

#include <string>
#include <vector>

class J3DShape;

class J3DPixelEngineBlock {
	friend class J3DMaterialFactory;

	J3DZMode mZMode;
	J3DAlphaCompare mAlphaCompare;
	J3DBlendMode mBlendMode;
	J3DFog mFog;

	bool mZCompLoc;
	bool mDither;

public:
	J3DPixelEngineBlock() : mZCompLoc(false), mDither(false) {}
};

class J3DLightBlock {
	friend class J3DMaterialFactory;

	EGXCullMode mCullMode;
	glm::vec4 mMatteColor[2];
	glm::vec4 mAmbientColor[2];

	std::vector<J3DColorChannel> mColorChannels;

public:
	J3DLightBlock() : mCullMode(EGXCullMode::None) {}
};

class J3DTexGenBlock {
	friend class J3DMaterialFactory;

	J3DNBTScaleInfo mNBTScale;
	std::vector<J3DTexCoordInfo> mTexCoordInfo;
	std::vector<J3DTexMatrixInfo> mTexMatrix;
};

class J3DTevBlock {
	friend class J3DMaterialFactory;

	std::vector<uint16_t> mTextureIndices;
	std::vector<J3DTevOrderInfo> mTevOrders;
	std::vector<J3DTevStageInfo> mTevStages;

	glm::vec4 mTevColors[4];
	glm::vec4 mTevKonstColors[4];

	EGXKonstColorSel mKonstColorSelection[16];
	EGXKonstAlphaSel mKonstAlphaSelection[16];
};

class J3DMaterial {
	friend class J3DMaterialFactory;

	std::string mName;
	int32_t mShaderProgram;
	J3DShape* mShape;

	EPixelEngineMode mPEMode;

	J3DPixelEngineBlock mPEBlock;
	J3DLightBlock mLightBlock;
	J3DTexGenBlock mTexGenBlock;
	J3DTevBlock mTevBlock;

public:
	J3DMaterial();
	~J3DMaterial();

	const J3DShape* GetShape() const { return mShape; }
	void SetShape(J3DShape* shape) { mShape = shape; }

	int32_t GetShaderProgram() const { return mShaderProgram; }
	bool GenerateShaders(const int32_t& jointCount);

	void Render();
};
