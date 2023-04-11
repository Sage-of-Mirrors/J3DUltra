#pragma once

#include "J3DEnvelope.hpp"
#include "J3DJoint.hpp"
#include "J3DShape.hpp"
#include "J3DMaterial.hpp"
#include "J3DBlock.hpp"
#include "J3DTexture.hpp"

#include <GXVertexData.hpp>
#include <GXGeometryData.hpp>
#include <cstdint>
#include <vector>
#include <memory>

class J3DModelLoader;
class J3DModelInstance;

class J3DModelData : public std::enable_shared_from_this<J3DModelData> {
	friend J3DModelLoader;
	friend J3DModelInstance;

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
	GXAttributeData mVertexData;

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
	GXGeometry mGeometry;

	std::vector<J3DVertexGX> mGXVertices;
	std::vector<J3DVertexGL> mGLVertices;
	std::vector<uint16_t> mIndices;

	// MAT3 data, materials
	std::vector<std::shared_ptr<J3DMaterial>> mMaterials;

	// TEX1 data, textures
	std::vector<std::shared_ptr<J3DTexture>> mTextures;

	// Calculated envelopes for the model's rest pose
	std::vector<glm::mat4> mRestPose;

	void MakeHierarchy(J3DJoint* const root, uint32_t& index);

	void CalculateRestPose();
	
	void CreateVBO();
	bool InitializeGL();

public:
	void Render(float deltaTime);
	J3DModelData() {}
	virtual ~J3DModelData() {}

	std::shared_ptr<J3DModelInstance> GetInstance();
	std::vector<glm::mat4> GetRestPose() const;
	std::vector<std::shared_ptr<J3DMaterial>> GetMaterials() const;
	std::vector<std::shared_ptr<J3DTexture>> GetTextures() const;
	const std::vector<J3DEnvelope>& GetJointEnvelopes() const { return mJointEnvelopes; }
};
