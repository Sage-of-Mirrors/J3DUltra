#pragma once

#include "J3D/Animation/J3DAnimationInstance.hpp"
#include "J3D/Animation/J3DDiscreteAnimationTrack.hpp"

#include "bstream.h"

#include <vector>

class J3DMaterial;

namespace J3DAnimation {
    struct J3DTexIndexAnimationData {
        std::string MaterialName;

        J3DDiscreteAnimationTrack Track;
        uint8_t TextureSlotIndex;
    };

    class J3DTexIndexAnimationInstance : public J3DAnimationInstance {
        std::vector<J3DTexIndexAnimationData> mEntries;

    public:
        J3DTexIndexAnimationInstance();

        virtual void Deserialize(bStream::CStream& stream) override;

        const std::vector<J3DTexIndexAnimationData>& GetEntries() const { return mEntries; }

        void ApplyAnimation(std::shared_ptr<J3DMaterial> material);
    };
}
