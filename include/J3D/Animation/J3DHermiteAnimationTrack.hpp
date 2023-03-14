#pragma once

#include <vector>

namespace J3DAnimation {
    struct J3DAnimationKey;

    class J3DHermiteAnimationTrack {
        std::vector<J3DAnimationKey> mKeys;

        float InterpolateValue(float time, const J3DAnimationKey* a, const J3DAnimationKey* b);

    public:
        J3DHermiteAnimationTrack();
        ~J3DHermiteAnimationTrack();

        float GetValue(float time);
    };
}
