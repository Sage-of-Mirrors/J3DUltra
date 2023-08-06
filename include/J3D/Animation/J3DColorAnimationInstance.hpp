#pragma once

#include "J3D/Animation/J3DAnimationInstance.hpp"
#include "J3D/Animation/J3DHermiteAnimationTrack.hpp"

#include "bstream.h"

#include <vector>

namespace J3DAnimation {
    struct J3DColorAnimationData {
        std::string MaterialName;

        J3DHermiteAnimationTrack RedTrack, GreenTrack, BlueTrack, AlphaTrack;
        uint8_t ColorIndex;
    };

    class J3DColorAnimationInstance : public J3DAnimationInstance {
        std::vector<J3DColorAnimationData> RegisterEntries;
        std::vector<J3DColorAnimationData> KonstEntries;

    public:
        J3DColorAnimationInstance();

        virtual void Deserialize(bStream::CStream& stream) override;
        virtual void ApplyAnimation(std::shared_ptr<J3DModelInstance> model) override;
    };
}
