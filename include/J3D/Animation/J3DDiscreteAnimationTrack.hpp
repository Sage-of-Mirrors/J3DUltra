#pragma once

#include <vector>

namespace J3DAnimation {
    struct J3DAnimationKey;

    class J3DDiscreteAnimationTrack {
        std::vector<J3DAnimationKey> mKeys;

    public:
        J3DDiscreteAnimationTrack();
        ~J3DDiscreteAnimationTrack();

        float GetValue(float time) const;
    };
}