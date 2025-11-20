#pragma once

#include "J3D/Animation/J3DAnimationInstance.hpp"
#include "J3D/Animation/J3DDiscreteAnimationTrack.hpp"

#include "bstream.h"

#include <glm/glm.hpp>
#include <vector>

class J3DJoint;

namespace J3DAnimation {
    struct J3DJointFullAnimationData {
        uint8_t JointIndex;

        J3DDiscreteAnimationTrack ScaleX, ScaleY, ScaleZ;
        J3DDiscreteAnimationTrack RotationX, RotationY, RotationZ;
        J3DDiscreteAnimationTrack TranslationX, TranslationY, TranslationZ;
    };

    class J3DJointFullAnimationInstance : public J3DAnimationInstance {
        std::vector<J3DJointFullAnimationData> mEntries;

        void ReadFloatComponentTrack(bStream::CStream& stream, J3DDiscreteAnimationTrack& track, uint32_t valueTableOffset);
        void ReadRotationComponentTrack(bStream::CStream& stream, J3DDiscreteAnimationTrack& track, uint32_t valueTableOffset, float scale);

    public:
        J3DJointFullAnimationInstance();

        virtual void Deserialize(bStream::CStream& stream) override;

        const std::vector<J3DJointFullAnimationData>& GetEntries() const { return mEntries; }
        uint32_t GetJointCount() const { return (uint32_t)mEntries.size(); }

        virtual std::vector<glm::mat4> GetTransformsAtFrame(float deltaTime);
    };
}
