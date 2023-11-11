#pragma once

#include "J3D/Data/J3DBlock.hpp"

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

        float mCurrentFrame;
        bool mIsPaused;
        bool mIsReversed;

    public:
        J3DAnimationInstance() : mLoopMode(ELoopMode::Once), mLength(0), mCurrentFrame(0), mIsPaused(false), mIsReversed(false) { }
        virtual ~J3DAnimationInstance() { }

        virtual void Deserialize(bStream::CStream& stream) = 0;

        uint16_t GetLength() const;

        float GetFrame() const;
        void SetFrame(uint16_t frame, bool pause = false);

        void SetPaused(bool paused);
        
        void Tick(float deltaTime);
    };
}