#include "J3D/Animation/J3DDiscreteAnimationTrack.hpp"
#include "J3D/Animation/J3DAnimationKey.hpp"

#include <cstdint>

J3DAnimation::J3DDiscreteAnimationTrack::J3DDiscreteAnimationTrack() {

}

J3DAnimation::J3DDiscreteAnimationTrack::~J3DDiscreteAnimationTrack() {

}

float J3DAnimation::J3DDiscreteAnimationTrack::GetValue(float time) const {
    uint32_t frame = static_cast<uint32_t>(time + 0.5f);

    if (frame >= mKeys.size()) {
        return mKeys[mKeys.size() - 1].Value;
    }
    else {
        return mKeys[frame].Value;
    }
}

void J3DAnimation::J3DDiscreteAnimationTrack::AddKey(J3DAnimation::J3DAnimationKey key) {
    mKeys.push_back(key);
}
