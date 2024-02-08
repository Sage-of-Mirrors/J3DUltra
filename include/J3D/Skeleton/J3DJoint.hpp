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

	std::vector<std::weak_ptr<J3DMaterial>> mMaterials;

public:

	virtual EJ3DNodeType GetType() { return EJ3DNodeType::Joint; }

	const std::string& GetJointName() const { return mJointName; }
	uint16_t GetJointID() const { return mJointID; }
	uint8_t GetMatrixFlag() const { return mMatrixFlag; }
	uint8_t GetAttachFlag() const { return mAttachFlag; }

	void AddMaterial(std::shared_ptr<J3DMaterial>& mat) { mMaterials.push_back(mat); }
	std::weak_ptr<J3DMaterial>& GetLastMaterial() { return mMaterials.back(); }

	glm::mat4 GetTransformMatrix();
};
