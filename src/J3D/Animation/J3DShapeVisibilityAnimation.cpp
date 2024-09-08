#include "J3D/Animation/J3DShapeVisibilityAnimation.hpp"
#include "J3D/Animation/J3DDiscreteAnimationTrack.hpp"

std::vector<bool> J3DAnimation::J3DShapeVisibilityAnimation::GetValues(float time) {
    std::vector<bool> values;
    values.reserve(mShapeTracks.size());

    for (const std::shared_ptr<J3DDiscreteAnimationTrack>& track : mShapeTracks) {
        values.push_back(static_cast<bool>(track->GetValue(time)));
    }

    return values;
}
