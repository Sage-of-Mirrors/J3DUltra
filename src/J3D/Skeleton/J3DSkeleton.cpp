#include "J3D/Skeleton/J3DSkeleton.hpp"
#include "J3D/Skeleton/J3DJoint.hpp"



J3DSkeleton::J3DSkeleton() {

}

std::shared_ptr<J3DJoint> J3DSkeleton::GetJoint(uint32_t index) {
    return index < mJoints.size() ? mJoints[index] : std::shared_ptr<J3DJoint>();
}

std::shared_ptr<J3DJoint> J3DSkeleton::GetJoint(std::string name) {
    for (uint32_t i = 0; i < mJoints.size(); i++) {
        if (mJoints[i]->GetJointName() == name) {
            return GetJoint(i);
        }
    }

    return std::shared_ptr<J3DJoint>();
}

void J3DSkeleton::CalculateRestPose() {
    for (int i = 0; i < mEnvelopeIndices.size(); i++) {
        if (mDrawBools[i] == false) {
            mRestPose.push_back(mJoints[mEnvelopeIndices[i]]->GetTransformMatrix());
        }
        else {
            glm::mat4 matrix = glm::zero<glm::mat4>();

            J3DEnvelope env = mJointEnvelopes[mEnvelopeIndices[i]];
            float weightTotal = 0.f;

            for (int j = 0; j < env.Weights.size(); j++) {
                uint32_t jointIndex = env.JointIndices[j];

                glm::mat4 ibm = mInverseBindMatrices[jointIndex];
                glm::mat4 jointTransform = mJoints[jointIndex]->GetTransformMatrix();

                matrix += (jointTransform * ibm) * env.Weights[j];
                weightTotal += env.Weights[j];
            }

            mRestPose.push_back(matrix);
        }
    }
    mRestPose.shrink_to_fit();
}

std::vector<glm::mat4> J3DSkeleton::CalculateAnimJointPose(const std::vector<glm::mat4>& transforms) {
    std::vector<glm::mat4> animTransforms;

    for (int i = 0; i < mEnvelopeIndices.size(); i++) {
        if (mDrawBools[i] == false) {
            animTransforms.push_back(transforms[mEnvelopeIndices[i]]);
        }
        else {
            glm::mat4 matrix = glm::zero<glm::mat4>();

            J3DEnvelope env = mJointEnvelopes[mEnvelopeIndices[i]];
            float weightTotal = 0.f;

            for (int j = 0; j < env.Weights.size(); j++) {
                uint32_t jointIndex = env.JointIndices[j];

                glm::mat4 ibm = mInverseBindMatrices[jointIndex];
                glm::mat4 jointTransform = transforms[jointIndex];

                matrix += (jointTransform * ibm) * env.Weights[j];
                weightTotal += env.Weights[j];
            }

            animTransforms.push_back(matrix);
        }
    }

    animTransforms.shrink_to_fit();

    return animTransforms;
}
