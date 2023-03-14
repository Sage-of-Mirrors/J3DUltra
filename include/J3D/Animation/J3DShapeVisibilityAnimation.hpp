#pragma once

#include <vector>
#include <memory>

namespace J3DAnimation {
    class J3DDiscreteAnimationTrack;

    class J3DShapeVisibilityAnimation {
        std::vector<std::shared_ptr<J3DDiscreteAnimationTrack>> mShapeTracks;

    public:
        std::vector<bool> GetValues(float time);
    };
}