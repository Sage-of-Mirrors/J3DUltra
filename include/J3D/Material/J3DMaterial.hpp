#pragma once

#include "J3DMaterialData.hpp"
#include <GXGeometryEnums.hpp>

#include <glm/glm.hpp>

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

	J3DPixelEngineBlock() : mZCompLoc(false), mDither(false) { }
};

struct J3DLightBlock {
	EGXCullMode mCullMode;
	glm::vec4 mMaterialColor[2];
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

	glm::vec4 mTevColors[4] {
		glm::one<glm::vec4>(),
		glm::one<glm::vec4>(),
		glm::one<glm::vec4>(),
		glm::one<glm::vec4>()
	};
	glm::vec4 mTevKonstColors[4] {
		glm::one<glm::vec4>(),
		glm::one<glm::vec4>(),
		glm::one<glm::vec4>(),
		glm::one<glm::vec4>()
	};

	EGXKonstColorSel mKonstColorSelection[16];
	EGXKonstAlphaSel mKonstAlphaSelection[16];
};

struct J3DIndirectBlock {
	bool mEnabled;
	uint32_t mNumStages;

	std::vector<std::shared_ptr<J3DIndirectTexOrderInfo>> mIndirectTexOrders;
	std::vector<std::shared_ptr<J3DIndirectTexMatrixInfo>> mIndirectTexMatrices;
	std::vector<std::shared_ptr<J3DIndirectTexScaleInfo>> mIndirectTexCoordScales;
	std::vector<std::shared_ptr<J3DIndirectTevStageInfo>> mIndirectTevStages;

	J3DIndirectBlock() : mEnabled(false) { }
};

class J3DMaterial {
	int32_t mShaderProgram;
	const GXShape* mShape;

	glm::mat4 TexMatrices[10]{};

	static std::atomic<uint16_t> sMaterialIdSrc;

	uint16_t mMaterialId;
	bool bSelected;

	void BindJ3DShader(const std::vector<std::shared_ptr<struct J3DTexture>>& textures);
	void ConfigureGLState();

public:
	J3DMaterial();
	~J3DMaterial();

	std::string Name;
	EPixelEngineMode PEMode;

	J3DPixelEngineBlock PEBlock;
	J3DLightBlock LightBlock;
	J3DTexGenBlock TexGenBlock;
	std::shared_ptr<J3DTevBlock> TevBlock;
	std::shared_ptr<J3DIndirectBlock> IndirectBlock;

	int TEVStageGenMax;

	bool AreRegisterColorsAnimating;
	glm::vec4 AnimationRegisterColors[4];
	glm::vec4 AnimationKonstColors[4];

	bool AreTexIndicesAnimating;
	uint16_t AnimationTexIndices[8]{};

	bool AreTexMatricesAnimating;
	J3DTexMatrixInfo AnimationTexMatrixInfo[10]{};

	void CalculateTexMatrices(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

	const GXShape* GetShape() const { return mShape; }
	void SetShape(const GXShape* shape) { mShape = shape; }

	int32_t GetShaderProgram() const { return mShaderProgram; }
	bool GenerateShaders();

	void Render(const std::vector<std::shared_ptr<struct J3DTexture>>& textures, uint32_t shaderOverride = 0);

	uint16_t GetMaterialId() const { return mMaterialId; }

	bool IsSelected() const { return bSelected; }
	void SetSelected(bool selected) { bSelected = selected; }
};
