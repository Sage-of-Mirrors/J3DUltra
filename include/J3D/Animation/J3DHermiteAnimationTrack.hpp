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

        float InterpolateValue(float time, const J3DAnimationKey* a, const J3DAnimationKey* b);

    public:
        J3DHermiteAnimationTrack();
        ~J3DHermiteAnimationTrack();

        void Deserialize(bStream::CStream& stream, uint32_t valueTableOffset, float valueDivisor = 1.0f);

        float GetValue(float time);
    };
}
