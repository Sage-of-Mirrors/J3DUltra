#include "J3D/Animation/J3DAnimationInstance.hpp"

uint16_t J3DAnimation::J3DAnimationInstance::GetLength() const {
    return mLength;
}

float J3DAnimation::J3DAnimationInstance::GetFrame() const {
    return mCurrentFrame * 30.0f;
}

void J3DAnimation::J3DAnimationInstance::SetFrame(uint16_t frame, bool pause) {
    mCurrentFrame = frame / 30.0f;
    mIsPaused = pause;
}

void J3DAnimation::J3DAnimationInstance::SetPaused(bool paused) {
    mIsPaused = paused;
}

void J3DAnimation::J3DAnimationInstance::Tick(float deltaTime) {
    if (mIsPaused) {
        return;
    }

    if (mIsReversed) {
        mCurrentFrame -= deltaTime;
        if (GetFrame() <= 0) {
            switch (mLoopMode) {
            default:
            case ELoopMode::Yoyo_Once:
                SetFrame(0, true);
                mIsReversed = false;
                break;
            case ELoopMode::Yoyo_Loop:
                SetFrame(0, false);
                mIsReversed = false;
                break;
            }
        }
        return;
    }

    mCurrentFrame += deltaTime;
    if (GetFrame() >= mLength) {
        switch (mLoopMode) {
        case ELoopMode::Once:
            SetFrame(mLength, true);
            break;
        case ELoopMode::Once_Reset:
            SetFrame(0, true);
            break;
        case ELoopMode::Yoyo_Once:
        case ELoopMode::Yoyo_Loop:
            SetFrame(mLength, false);
            mIsReversed = true;
            break;
        case ELoopMode::Loop:
        default:
            SetFrame(0);
            break;
        }
    }
}