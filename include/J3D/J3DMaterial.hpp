#pragma once

#include "J3D/J3DMaterialData.hpp"
#include "GX/GXEnum.hpp"

#include <string>

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

class J3DMaterial {
	friend class J3DMaterialFactory;

	std::string mName;
	int32_t mShaderProgram;
	J3DShape* mShape;

	EPixelEngineMode mPEMode;
	EGXCullMode mCullMode;

	J3DPixelEngineBlock mPEBlock;

public:
	J3DMaterial();
	~J3DMaterial();

	const J3DShape* GetShape() const { return mShape; }
	void SetShape(J3DShape* shape) { mShape = shape; }

	int32_t GetShaderProgram() const { return mShaderProgram; }
	bool GenerateShaders(const int32_t& jointCount);

	void Render();
};
