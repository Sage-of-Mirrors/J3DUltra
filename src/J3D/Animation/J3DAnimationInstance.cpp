#include "J3D/Animation/J3DAnimationInstance.hpp"

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

     mCurrentFrame += deltaTime;

     if (GetFrame() >= mLength) {
         switch (mLoopMode) {
         case ELoopMode::Once:
             mIsPaused = true;
             break;
         case ELoopMode::Once_Reset:
             mCurrentFrame = 0;
             mIsPaused = true;
             break;
         case ELoopMode::Loop:
         default:
             mCurrentFrame = 0;
             break;
         }
     }
}