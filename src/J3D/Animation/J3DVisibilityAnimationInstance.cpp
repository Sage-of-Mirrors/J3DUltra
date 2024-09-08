#include "J3D/Animation/J3DVisibilityAnimationInstance.hpp"
#include "J3D/Animation/J3DAnimationKey.hpp"

J3DAnimation::J3DVisibilityAnimationInstance::J3DVisibilityAnimationInstance() {

}

void J3DAnimation::J3DVisibilityAnimationInstance::ReadBooleanComponentTrack(
    bStream::CStream& stream, J3DDiscreteAnimationTrack& track, uint32_t valueTableOffset)
{
    uint16_t keyCount = stream.readUInt16();
    uint16_t firstKeyIndex = stream.readUInt16();

    size_t currentStreamPos = stream.tell();
    stream.seek(valueTableOffset + firstKeyIndex * sizeof(float));

    for (uint16_t i = 0; i < keyCount; i++) {
        J3DAnimationKey newKey;

        newKey.Value = stream.readUInt8();

        track.AddKey(newKey);
    }

    stream.seek(currentStreamPos);
}

void J3DAnimation::J3DVisibilityAnimationInstance::Deserialize(bStream::CStream& stream) {
    size_t currentStreamPos = stream.tell();

    // Deserialize counts and offsets from the block
    J3DVisibilityBlock visBlock;
    visBlock.Deserialize(&stream);

    mLength = visBlock.Length;
    mLoopMode = static_cast<ELoopMode>(visBlock.LoopMode);

    size_t fullTrackCount = visBlock.TrackCount;

    mEntries.reserve(fullTrackCount);

    stream.seek(visBlock.TrackTableOffset);
    for (int i = 0; i < fullTrackCount; i++) {
        J3DVisibilityAnimationData animData;
        animData.ShapeIndex = i;

        ReadBooleanComponentTrack(stream, animData.Visibility, visBlock.BooleanTableOffset);

        mEntries.push_back(animData);
    }

    stream.seek(currentStreamPos + visBlock.BlockSize);
}

bool J3DAnimation::J3DVisibilityAnimationInstance::GetVisibilityAtFrame(uint32_t shapeIdx, float deltaTime) {
    if (shapeIdx >= mEntries.size()) {
        return true;
    }

    float frameTime = GetFrame();
    return static_cast<bool>(mEntries[shapeIdx].Visibility.GetValue(frameTime));
}
