#pragma once

#include <filesystem>
#include <memory>

namespace bStream {
    class CStream;
}

namespace J3DAnimation {
    class J3DAnimationInstance;
    class J3DColorAnimationInstance;

    class J3DAnimationLoader {
    protected:
        std::shared_ptr<J3DAnimationInstance> mAnimInstance;

    public:
        J3DAnimationLoader();

        template<typename T>
        std::shared_ptr<T> LoadAnimation(std::filesystem::path filePath) {
            std::shared_ptr<J3DAnimationInstance> loadedAnimation = LoadAnimation(filePath);
            return std::dynamic_pointer_cast<T>(loadedAnimation);
        }

        std::shared_ptr<J3DAnimationInstance> LoadAnimation(std::filesystem::path filePath);
        std::shared_ptr<J3DAnimationInstance> LoadAnimation(void* buffer, uint32_t size);
        std::shared_ptr<J3DAnimationInstance> LoadAnimation(bStream::CStream& stream);
    };
}
