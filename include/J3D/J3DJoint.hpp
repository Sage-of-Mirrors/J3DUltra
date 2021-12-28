#pragma once

#include <string>
#include "J3DNode.hpp"
#include "J3DTransform.hpp"
#include "glm/glm.hpp"

class J3DModelLoader;

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

public:

	virtual EJ3DNodeType GetType() { return EJ3DNodeType::Joint; }

	std::string GetJointName() { return mJointName; }
	uint16_t GetJointID() { return mJointID; }
};
