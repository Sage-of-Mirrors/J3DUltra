#pragma once

#include "J3D/Animation/J3DAnimationInstance.hpp"
#include "J3D/Animation/J3DHermiteAnimationTrack.hpp"

#include "bstream.h"

#include <glm/glm.hpp>
#include <vector>

class J3DJoint;

namespace J3DAnimation {
    struct J3DJointAnimationData {
        uint8_t JointIndex;

        J3DHermiteAnimationTrack ScaleX, ScaleY, ScaleZ;
        J3DHermiteAnimationTrack RotationX, RotationY, RotationZ;
        J3DHermiteAnimationTrack TranslationX, TranslationY, TranslationZ;
    };

    class J3DJointAnimationInstance : public J3DAnimationInstance {
        std::vector<J3DJointAnimationData> mEntries;

        void ReadFloatComponentTrack(bStream::CStream& stream, J3DHermiteAnimationTrack& track, uint32_t valueTableOffset);
        void ReadRotationComponentTrack(bStream::CStream& stream, J3DHermiteAnimationTrack& track, uint32_t valueTableOffset, float scale);

    public:
        J3DJointAnimationInstance();

        virtual void Deserialize(bStream::CStream& stream) override;

        const std::vector<J3DJointAnimationData>& GetEntries() const { return mEntries; }

        virtual std::vector<glm::mat4> GetTransformsAtFrame(float deltaTime);
    };
}