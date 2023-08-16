#pragma once

#include "J3D/Animation/J3DAnimationInstance.hpp"
#include "J3D/Animation/J3DHermiteAnimationTrack.hpp"

#include "bstream.h"

#include <glm/glm.hpp>
#include <vector>

class J3DMaterial;

namespace J3DAnimation {
    struct J3DTexMatrixAnimationData {
        std::string MaterialName;
        uint8_t TexGenIndex;

        J3DHermiteAnimationTrack ScaleS, ScaleT, ScaleQ;
        J3DHermiteAnimationTrack RotationS, RotationT, RotationQ;
        J3DHermiteAnimationTrack TranslationS, TranslationT, TranslationQ;

        glm::vec3 Origin;
    };

    class J3DTexMatrixAnimationInstance : public J3DAnimationInstance {
        std::vector<J3DTexMatrixAnimationData> mEntries;

        void ReadFloatComponentTrack(bStream::CStream& stream, J3DHermiteAnimationTrack& track, uint32_t valueTableOffset);
        void ReadRotationComponentTrack(bStream::CStream& stream, J3DHermiteAnimationTrack& track, uint32_t valueTableOffset, float scale);

    public:
        J3DTexMatrixAnimationInstance();

        virtual void Deserialize(bStream::CStream& stream) override;

        const std::vector<J3DTexMatrixAnimationData>& GetEntries() const { return mEntries; }

        void ApplyAnimation(std::shared_ptr<J3DMaterial> material);
    };
}