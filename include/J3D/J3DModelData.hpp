#pragma once

#include <cstdint>
#include <vector>

#include "J3DVertexData.hpp"
#include "J3DEnvelope.hpp"
#include "J3DJoint.hpp"
#include "J3DShape.hpp"
#include "J3DMaterial.hpp"
#include "J3DBlock.hpp"

class J3DModelLoader;

class J3DModelData {
	friend J3DModelLoader;

	// Rendering stuff
	bool mGLInitialized = false;
	uint32_t mVAO = UINT32_MAX;
	uint32_t mVBO = UINT32_MAX;
	uint32_t mIBO = UINT32_MAX;

	// INF1 data, hierarchy and misc. info
	uint32_t mFlags;
	uint32_t mMatrixGroupCount;
	std::vector<J3DModelHierarchy> mHierarchyNodes;

	// VTX1 data, vertex buffers
	J3DVertexData mVertexData;

	// EVP1 data, skinning envelopes
	std::vector<J3DEnvelope> mJointEnvelopes;
	std::vector<glm::mat4x4> mInverseBindMatrices;

	// DRW1 data, maps shapes to skinning envelopes
	std::vector<bool> mDrawBools;
	std::vector<uint16_t> mEnvelopeIndices;

	// JNT1 data, joints
	J3DJoint* mRootJoint;
	std::vector<J3DJoint*> mJoints;

	// SHP1 data, geometry
	std::vector<J3DShape*> mShapes;

	std::vector<J3DVertexGX> mGXVertices;
	std::vector<J3DVertexGL> mGLVertices;
	std::vector<uint16_t> mIndices;

	// MAT3 data, materials
	std::vector<J3DMaterial*> mMaterials;

	void MakeHierarchy(J3DJoint* const root, uint32_t& index);
	void ConvertGXVerticesToGL();

	bool InitializeGL();

public:
	J3DModelData() {}
	virtual ~J3DModelData() {}

	void Render(float deltaTime);
};
