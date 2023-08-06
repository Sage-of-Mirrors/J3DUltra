#include "J3D/Animation/J3DHermiteAnimationTrack.hpp"
#include "J3D/Animation/J3DAnimationKey.hpp"

#include "bstream.h"

#include <glm/glm.hpp>

const glm::mat4 HERMITE_MTX(
    2.0f, -2.0f, 1.0f, 1.0f,
    -3.0f, 3.0f, -2.0f, -1.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 0.0f
);

J3DAnimation::J3DHermiteAnimationTrack::J3DHermiteAnimationTrack() {

}

J3DAnimation::J3DHermiteAnimationTrack::~J3DHermiteAnimationTrack() {

}

void J3DAnimation::J3DHermiteAnimationTrack::Deserialize(bStream::CStream& stream, uint32_t valueTableOffset, float valueDivisor) {
    uint16_t keyCount = stream.readUInt16();
    uint16_t firstKeyIndex = stream.readUInt16();
    ETangentMode tangentMode = static_cast<ETangentMode>(stream.readUInt16());

    size_t currentStreamPos = stream.tell();
    stream.seek(valueTableOffset + firstKeyIndex * sizeof(uint16_t));

    if (keyCount == 1) {
        J3DAnimationKey newKey;
        newKey.Value = static_cast<float>(stream.readUInt16());

        mKeys.push_back(newKey);
    }
    else {
        for (uint16_t i = 0; i < keyCount; i++) {
            J3DAnimationKey newKey;

            switch (tangentMode) {
                case ETangentMode::Symmetric:
                    newKey.Time = static_cast<float>(stream.readUInt16());
                    newKey.Value = static_cast<float>(stream.readUInt16()) / valueDivisor;
                    newKey.InTangent = static_cast<float>(stream.readUInt16()) / UINT16_MAX;
                    newKey.OutTangent = newKey.InTangent;
                    break;
                case ETangentMode::Piecewise:
                    newKey.Time = static_cast<float>(stream.readUInt16());
                    newKey.Value = static_cast<float>(stream.readUInt16()) / valueDivisor;
                    newKey.InTangent = static_cast<float>(stream.readUInt16()) / UINT16_MAX;
                    newKey.OutTangent = static_cast<float>(stream.readUInt16()) / UINT16_MAX;
                    break;
                default:
                    break;
            }

            mKeys.push_back(newKey);
        }
    }

    stream.seek(currentStreamPos);
}

float J3DAnimation::J3DHermiteAnimationTrack::GetValue(float time) {
    if (time == 0.0f) {
        return mKeys[0].Value;
    }

    const J3DAnimationKey* firstKey = nullptr;
    const J3DAnimationKey* secondKey = nullptr;

    for (const J3DAnimationKey& k : mKeys) {
        if (k.Time <= time) {
            firstKey = &k;
        }
        else if (k.Time >= time) {
            secondKey = &k;
        }
    }

    if (secondKey == nullptr) {
        secondKey = &mKeys[0];
    }

    return InterpolateValue(time, firstKey, secondKey);
}

float J3DAnimation::J3DHermiteAnimationTrack::InterpolateValue(float time, const J3DAnimationKey* a, const J3DAnimationKey* b) {
    float framesBetweenKeys = b->Time - a->Time;
    
    glm::vec4 timeParameters = glm::vec4(time * time * time, time * time, time, 1.0f);
    glm::vec4 valueParameters = glm::vec4(a->Value, b->Value, a->OutTangent * framesBetweenKeys, b->InTangent * framesBetweenKeys);

    glm::vec4 transform = HERMITE_MTX * timeParameters;
    glm::vec4 result = result * valueParameters;

    return result.x + result.y + result.z + result.w;
}
