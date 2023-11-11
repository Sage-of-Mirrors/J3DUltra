#pragma once

#include "J3DNode.hpp"
#include "J3D/Util/J3DTransform.hpp"
#include "J3D/Util/J3DUtil.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <memory>

class J3DModelLoader;
class J3DMaterial;

enum class EJ3DJointBBoardType : uint8_t {
	None,
	XY,
	X
};

class J3DJoint : public J3DNode {
	friend J3DModelLoader;

	std::string mJointName;

	uint16_t mJointID;
	uint8_t mMatrixFlag;
	uint8_t mAttachFlag;
	J3DTransformInfo mTransform;

	float mBoundingSphereRadius;
	glm::vec3 mBoundingBoxMin;
	glm::vec3 mBoundingBoxMax;

	shared_vector<J3DMaterial> mMaterials;

public:

	virtual EJ3DNodeType GetType() { return EJ3DNodeType::Joint; }

	std::string GetJointName() { return mJointName; }
	uint16_t GetJointID() { return mJointID; }

	void AddMaterial(std::shared_ptr<J3DMaterial>& mat) { mMaterials.push_back(mat); }
	std::shared_ptr<J3DMaterial>& GetLastMaterial() { return mMaterials.back(); }

	glm::mat4 GetTransformMatrix();

	void RenderRecursive(std::vector<std::shared_ptr<struct J3DTexture>>& textures);
};
