#pragma once

#include "J3D/J3DMaterialData.hpp"

#include <GXGeometryEnums.hpp>
#include <string>
#include <vector>
#include <memory>

class GXShape;

struct J3DPixelEngineBlock {
	J3DZMode mZMode;
	J3DAlphaCompare mAlphaCompare;
	J3DBlendMode mBlendMode;
	J3DFog mFog;

	bool mZCompLoc;
	bool mDither;

public:
	J3DPixelEngineBlock() : mZCompLoc(false), mDither(false) {}
};

struct J3DLightBlock {
	EGXCullMode mCullMode;
	glm::vec4 mMatteColor[2];
	glm::vec4 mAmbientColor[2];

	std::vector<std::shared_ptr<J3DColorChannel>> mColorChannels;

public:
	J3DLightBlock() : mCullMode(EGXCullMode::None) {}
};

struct J3DTexGenBlock {
	J3DNBTScaleInfo mNBTScale;
	std::vector<std::shared_ptr<J3DTexCoordInfo>> mTexCoordInfo;
	std::vector<std::shared_ptr<J3DTexCoordInfo>> mTexCoord2Info;
	std::vector<std::shared_ptr<J3DTexMatrixInfo>> mTexMatrix;
};

struct J3DTevBlock {
	std::vector<uint16_t> mTextureIndices;
	std::vector<std::shared_ptr<J3DTevOrderInfo>> mTevOrders;
	std::vector<std::shared_ptr<J3DTevStageInfo>> mTevStages;

	glm::vec4 mTevColors[4];
	glm::vec4 mTevKonstColors[4];

	EGXKonstColorSel mKonstColorSelection[16];
	EGXKonstAlphaSel mKonstAlphaSelection[16];
};

class J3DMaterial {
	int32_t mShaderProgram;
	const GXShape* mShape;

public:
	J3DMaterial();
	~J3DMaterial();

	std::string Name;
	EPixelEngineMode PEMode;

	J3DPixelEngineBlock PEBlock;
	J3DLightBlock LightBlock;
	J3DTexGenBlock TexGenBlock;
	std::shared_ptr<J3DTevBlock> TevBlock;

	int TEVStageGenMax;

	const GXShape* GetShape() const { return mShape; }
	void SetShape(const GXShape* shape) { mShape = shape; }

	int32_t GetShaderProgram() const { return mShaderProgram; }
	bool GenerateShaders();

	void Render(std::vector<uint32_t>& textureHandles);
};
