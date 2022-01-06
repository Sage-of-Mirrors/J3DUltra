#pragma once

#include <cstdint>
#include <vector>

#include "J3DVertexData.hpp"
#include "J3DEnvelope.hpp"
#include "J3DJoint.hpp"
#include "J3DShape.hpp"

class J3DModelLoader;

class J3DModelData {
	friend J3DModelLoader;

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

	void MakeHierarchy(void* const root, std::vector<J3DModelHierarchy>* nodes) {}

public:
	J3DModelData() {}
	virtual ~J3DModelData() {}


};
