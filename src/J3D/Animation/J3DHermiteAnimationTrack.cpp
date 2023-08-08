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

float J3DAnimation::J3DHermiteAnimationTrack::GetValue(float time) const {
    if (mKeys.size() == 1) {
        return mKeys[0].Value;
    }

    size_t index = 1;
    while (mKeys[index].Time < time) {
        index++;
        if (index >= mKeys.size()) {
            index = mKeys.size() - 1;
            time = mKeys[index].Time;

            break;
        }
    }

    const J3DAnimationKey* firstKey = &mKeys[index - 1];
    const J3DAnimationKey* secondKey = &mKeys[index];

    float frameTime = (time - firstKey->Time) / (secondKey->Time - firstKey->Time);
    return InterpolateValue(frameTime, firstKey, secondKey);
}

float J3DAnimation::J3DHermiteAnimationTrack::InterpolateValue(float time, const J3DAnimationKey* a, const J3DAnimationKey* b) const {
    float framesBetweenKeys = b->Time - a->Time;
    
    glm::vec4 timeParameters = glm::vec4(time * time * time, time * time, time, 1.0f);
    glm::vec4 valueParameters = glm::vec4(a->Value, b->Value, a->OutTangent * framesBetweenKeys, b->InTangent * framesBetweenKeys);

    glm::vec4 transform = HERMITE_MTX * timeParameters;
    glm::vec4 result = transform * valueParameters;

    return result.x + result.y + result.z + result.w;
}

void J3DAnimation::J3DHermiteAnimationTrack::AddKey(J3DAnimation::J3DAnimationKey key) {
    mKeys.push_back(key);
}
