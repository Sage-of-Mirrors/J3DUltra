#pragma once

#include "J3D/J3DBlock.hpp"

#include <stdint.h>
#include <memory>

class J3DModelInstance;

namespace bStream {
    class CStream;
}

namespace J3DAnimation {
    enum class ELoopMode {
        Once = 0,
        Once_Reset = 1,
        Loop = 2,
        Yoyo_Once = 3,
        Yoyo_Loop = 4
    };

    class J3DAnimationInstance {
    protected:
        ELoopMode mLoopMode;
        uint16_t mLength;

        uint16_t mCurrentFrame;
        bool mIsPaused;

    public:
        J3DAnimationInstance() : mLoopMode(ELoopMode::Once), mLength(0), mCurrentFrame(0), mIsPaused(false) { }
        virtual ~J3DAnimationInstance() { }

        virtual void Deserialize(bStream::CStream& stream) = 0;
        virtual void ApplyAnimation(std::shared_ptr<J3DModelInstance> model) = 0;

        uint16_t GetFrame() { return mCurrentFrame; }
        void SetFrame(uint16_t frame, bool pause = false) { mCurrentFrame = frame; mIsPaused = pause; }
    };
}