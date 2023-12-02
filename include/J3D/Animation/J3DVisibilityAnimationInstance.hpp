#pragma once

#include "J3D/Animation/J3DAnimationInstance.hpp"
#include "J3D/Animation/J3DDiscreteAnimationTrack.hpp"

#include "bstream.h"

#include <glm/glm.hpp>
#include <vector>


namespace J3DAnimation {
    struct J3DVisibilityAnimationData {
        uint8_t ShapeIndex;

        J3DDiscreteAnimationTrack Visibility;
    };

    class J3DVisibilityAnimationInstance : public J3DAnimationInstance {
        std::vector<J3DVisibilityAnimationData> mEntries;

        void ReadBooleanComponentTrack(bStream::CStream& stream, J3DDiscreteAnimationTrack& track, uint32_t valueTableOffset);

    public:
        J3DVisibilityAnimationInstance();

        virtual void Deserialize(bStream::CStream& stream) override;

        const std::vector<J3DVisibilityAnimationData>& GetEntries() const { return mEntries; }
        
        bool GetVisibilityAtFrame(uint32_t shapeIdx, float deltaTime);
    };
}

