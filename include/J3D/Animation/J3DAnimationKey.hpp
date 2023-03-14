#pragma once

namespace J3DAnimation {
    struct J3DAnimationKey {
        float Time;
        float InTangent;
        float Value;
        float OutTangent;

        J3DAnimationKey();
    };
}
