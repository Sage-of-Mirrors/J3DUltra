#include "J3D/Animation/J3DHermiteAnimationTrack.hpp"
#include "J3D/Animation/J3DAnimationKey.hpp"

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
