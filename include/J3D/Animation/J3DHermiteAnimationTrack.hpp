#pragma once

#include "J3D/Animation/J3DAnimationKey.hpp"

#include <vector>

namespace bStream {
    class CStream;
}

namespace J3DAnimation {
    enum class ETangentMode {
        Symmetric,
        Piecewise
    };

    class J3DHermiteAnimationTrack {
        std::vector<J3DAnimationKey> mKeys;

        float InterpolateValue(float time, const J3DAnimationKey* a, const J3DAnimationKey* b) const;

    public:
        J3DHermiteAnimationTrack();
        ~J3DHermiteAnimationTrack();

        void AddKey(J3DAnimationKey key);
        float GetValue(float time) const;
    };
}
