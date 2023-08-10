#include "J3D/Animation/J3DTexIndexAnimationInstance.hpp"

#include "J3D/Animation/J3DAnimationKey.hpp"
#include "J3D/J3DModelInstance.hpp"
#include "J3D/J3DNameTable.hpp"
#include "J3D/J3DMaterial.hpp"

#include <memory>
#include <algorithm>

J3DAnimation::J3DTexIndexAnimationInstance::J3DTexIndexAnimationInstance() {

}

void J3DAnimation::J3DTexIndexAnimationInstance::Deserialize(bStream::CStream& stream) {
    size_t currentStreamPos = stream.tell();

    // Deserialize counts and offsets from the block
    J3DTexIndexKeyBlock indexKeyBlock;
    indexKeyBlock.Deserialize(&stream);

    mLength = indexKeyBlock.Length;
    mLoopMode = static_cast<ELoopMode>(indexKeyBlock.LoopMode);

    // Deserialize the material names for register colors
    stream.seek(indexKeyBlock.MaterialNameTableOffset);
    J3DNameTable materialNames;
    materialNames.Deserialize(&stream);

    stream.seek(indexKeyBlock.TrackTableOffset);
    for (int i = 0; i < indexKeyBlock.TrackCount; i++) {
        J3DTexIndexAnimationData animData;
        animData.MaterialName = materialNames.GetName(i);

        uint16_t keyCount = stream.readUInt16();
        uint16_t firstKeyIndex = stream.readUInt16();
        
        animData.TextureSlotIndex = stream.readUInt8();
        stream.skip(3);

        size_t currentStreamPos = stream.tell();
        stream.seek(indexKeyBlock.IndexTableOffset + firstKeyIndex * sizeof(uint16_t));

        for (int i = 0; i < keyCount; i++) {
            J3DAnimationKey newKey;
            newKey.Value = stream.readUInt16();

            animData.Track.AddKey(newKey);
        }

        mEntries.push_back(animData);
        stream.seek(currentStreamPos);
    }

    stream.seek(currentStreamPos + indexKeyBlock.BlockSize);
}

void J3DAnimation::J3DTexIndexAnimationInstance::ApplyAnimation(std::shared_ptr<J3DMaterial> material) {
    std::vector<J3DAnimation::J3DTexIndexAnimationData>::const_iterator matFindResult = std::find_if(
        mEntries.begin(),
        mEntries.end(),
        [&material](const J3DAnimation::J3DTexIndexAnimationData& v) { return v.MaterialName == material->Name; }
    );

    if (matFindResult == mEntries.end()) {
        return;
    }

    material->AreTexIndicesAnimating = true;

    J3DAnimation::J3DTexIndexAnimationData data = *matFindResult;
    //material->AnimationTexIndices[i] = material->TevBlock->mTextureIndices[i];
    material->AnimationTexIndices[data.TextureSlotIndex] = data.Track.GetValue(GetFrame());
}
