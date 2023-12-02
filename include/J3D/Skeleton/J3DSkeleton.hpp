#pragma once

#include "J3D/Util/J3DUtil.hpp"

#include "J3D/Skeleton/J3DEnvelope.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <memory>


struct J3DEnvelope;
class J3DJoint;

class J3DSkeleton {
	friend class J3DModelLoader;

	// EVP1 data, skinning envelopes
	std::vector<J3DEnvelope> mJointEnvelopes;
	std::vector<glm::mat4x4> mInverseBindMatrices;

	// DRW1 data, maps shapes to skinning envelopes
	std::vector<bool> mDrawBools;
	std::vector<uint16_t> mEnvelopeIndices;

	// JNT1 data, joints
	std::shared_ptr<J3DJoint> mRootJoint;
	shared_vector<J3DJoint> mJoints;

	// Calculated envelopes for the model's rest pose
	std::vector<glm::mat4> mRestPose;

public:
	J3DSkeleton();

	std::shared_ptr<J3DJoint> GetJoint(uint32_t index);
	std::shared_ptr<J3DJoint> GetJoint(std::string name);
	shared_vector<J3DJoint>& GetJoints() { return mJoints; }

	const std::vector<glm::mat4>& GetRestPose() const { return mRestPose; }
	const std::vector<J3DEnvelope>& GetJointEnvelopes() const { return mJointEnvelopes; }

	void SetRootJoint(std::shared_ptr<J3DJoint> jnt) { mRootJoint = jnt; }

	void CalculateRestPose();
	std::vector<glm::mat4> CalculateAnimJointPose(const std::vector<glm::mat4>& transforms);
};