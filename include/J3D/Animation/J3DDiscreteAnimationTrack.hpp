#pragma once

#include <vector>

namespace J3DAnimation {
    struct J3DAnimationKey;

    class J3DDiscreteAnimationTrack {
        std::vector<J3DAnimationKey> mKeys;

    public:
        J3DDiscreteAnimationTrack();
        ~J3DDiscreteAnimationTrack();

        void AddKey(J3DAnimationKey key);
        float GetValue(float time) const;
    };
}