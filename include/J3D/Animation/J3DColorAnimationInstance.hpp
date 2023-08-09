#pragma once

#include "J3D/Animation/J3DAnimationInstance.hpp"
#include "J3D/Animation/J3DHermiteAnimationTrack.hpp"

#include "bstream.h"

#include <vector>

class J3DMaterial;

namespace J3DAnimation {
    struct J3DColorAnimationData {
        std::string MaterialName;

        J3DHermiteAnimationTrack RedTrack, GreenTrack, BlueTrack, AlphaTrack;
        uint8_t ColorIndex;
    };

    class J3DColorAnimationInstance : public J3DAnimationInstance {
        std::vector<J3DColorAnimationData> RegisterEntries;
        std::vector<J3DColorAnimationData> KonstEntries;

        void ReadColorTrack(bStream::CStream& stream, J3DHermiteAnimationTrack& track, uint32_t valueTableOffset);

    public:
        J3DColorAnimationInstance();

        virtual void Deserialize(bStream::CStream& stream) override;

        const std::vector<J3DColorAnimationData>& GetRegisterEntries() const { return RegisterEntries; }
        const std::vector<J3DColorAnimationData>& GetKonstEntries() const { return KonstEntries; }

        void ApplyAnimation(std::shared_ptr<J3DMaterial> material);
    };
}
