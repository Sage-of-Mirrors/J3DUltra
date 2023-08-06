#include "J3D/Animation/J3DAnimationLoader.hpp"
#include "J3D/Animation/J3DAnimationInstance.hpp"
#include "J3D/Animation/J3DHermiteAnimationTrack.hpp"

#include "J3D/Animation/J3DColorAnimationInstance.hpp"

#include "J3D/J3DBlock.hpp"
#include "J3D/J3DData.hpp"
#include "bstream.h"

#include <iostream>

J3DAnimation::J3DAnimationLoader::J3DAnimationLoader() : mAnimInstance() {

}

std::shared_ptr<J3DAnimation::J3DAnimationInstance> J3DAnimation::J3DAnimationLoader::LoadAnimation(std::filesystem::path filePath) {
    if (!std::filesystem::exists(filePath)) {
        return nullptr;
    }

    bStream::CFileStream stream(filePath.string(), bStream::Big, bStream::In);
    return LoadAnimation(stream);
}

std::shared_ptr<J3DAnimation::J3DAnimationInstance> J3DAnimation::J3DAnimationLoader::LoadAnimation(void* buffer, uint32_t size) {
    if (buffer == nullptr || size <= 0) {
        return nullptr;
    }

    bStream::CMemoryStream stream(reinterpret_cast<uint8_t*>(buffer), size, bStream::Big, bStream::In);
    return LoadAnimation(stream);
}

std::shared_ptr<J3DAnimation::J3DAnimationInstance> J3DAnimation::J3DAnimationLoader::LoadAnimation(bStream::CStream& stream) {
    J3DDataBase header;
    header.Deserialize(&stream);

    EJ3DBlockType blockType = static_cast<EJ3DBlockType>(stream.peekUInt32(stream.tell()));
    switch (blockType) {
        // BRK
        case EJ3DBlockType::TRK1:
            mAnimInstance = std::make_shared<J3DColorAnimationInstance>();
            break;
        // BTK
        case EJ3DBlockType::TTK1:
        // BTP
        case EJ3DBlockType::TPT1:
        // BCK
        case EJ3DBlockType::ANK1:
        // BPK
        case EJ3DBlockType::PAK1:
        // BVA
        case EJ3DBlockType::VAF1:
        // Unknown
        default:
            std::cout << "Failed to load animation with block type \"" << static_cast<int>(blockType) << "\"." << std::endl;
            break;
    }

    if (mAnimInstance != nullptr) {
        mAnimInstance->Deserialize(stream);
    }

    return mAnimInstance;
}
