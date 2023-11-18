#pragma once

#include "J3DBlock.hpp"
#include "J3D/Skeleton/J3DEnvelope.hpp"
#include "J3D/Skeleton/J3DJoint.hpp"
#include "J3D/Geometry/J3DShape.hpp"
#include "J3D/Material//J3DMaterialTable.hpp"
#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Texture/J3DTexture.hpp"
#include "J3D/Util/J3DUtil.hpp"

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

	std::shared_ptr<J3DMaterialTable> mMaterialTable;

	// Calculated envelopes for the model's rest pose
	std::vector<glm::mat4> mRestPose;

	void MakeHierarchy(J3DJoint* const root, uint32_t& index);

	void CalculateRestPose();
	
	void CreateVBO();
	bool InitializeGL();

public:
	J3DModelData();
	virtual ~J3DModelData();

	std::shared_ptr<J3DModelInstance> CreateInstance();

	std::vector<glm::mat4> GetRestPose() const;
	const std::vector<J3DEnvelope>& GetJointEnvelopes() const { return mJointEnvelopes; }

	/* Returns the material at the given index, or an empty shared_ptr if it does not exist. */
	std::shared_ptr<J3DMaterial> GetMaterial(uint32_t idx) { return mMaterialTable->GetMaterial(idx); }
	/* Returns the material with the given name, or an empty shared_ptr if it does not exist. */
	std::shared_ptr<J3DMaterial> GetMaterial(std::string name) { return mMaterialTable->GetMaterial(name); }
	/* Returns this model's list of default materials. */
	shared_vector<J3DMaterial>& GetMaterials() { return mMaterialTable->GetMaterials(); }

	/* Returns the texture at the given index, or an empty shared_ptr if it does not exist. */
	std::shared_ptr<J3DTexture> GetTexture(uint32_t idx) { return mMaterialTable->GetTexture(idx); }
	/* Returns the texture with the given name, or an empty shared_ptr if it does not exist. */
	std::shared_ptr<J3DTexture> GetTexture(std::string name) { return mMaterialTable->GetTexture(name); }
	/* Returns this model's list of default textures. */
	shared_vector<J3DTexture>& GetTextures() { return mMaterialTable->GetTextures(); }

	bool SetTexture(uint32_t idx, uint32_t width, uint32_t height, uint8_t* data, uint32_t size);
	bool SetTexture(std::string name, uint32_t width, uint32_t height, uint8_t* data, uint32_t size);

	void BindVAO();
	void UnbindVAO();
};
