#include "J3D/Animation/J3DJointFullAnimationInstance.hpp"
#include "J3D/Animation/J3DAnimationKey.hpp"

#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


J3DAnimation::J3DJointFullAnimationInstance::J3DJointFullAnimationInstance() {

}

void J3DAnimation::J3DJointFullAnimationInstance::ReadFloatComponentTrack(bStream::CStream& stream, J3DDiscreteAnimationTrack& track, uint32_t valueTableOffset) {
    uint16_t keyCount = stream.readUInt16();
    uint16_t firstKeyIndex = stream.readUInt16();

    size_t currentStreamPos = stream.tell();
    stream.seek(valueTableOffset + firstKeyIndex * sizeof(float));

    if (keyCount == 1) {
        J3DAnimationKey newKey;
        newKey.Value = stream.readFloat();

        track.AddKey(newKey);
    }
    else {
        for (uint16_t i = 0; i < keyCount; i++) {
            J3DAnimationKey newKey;

            //newKey.Time = stream.readFloat();
            newKey.Value = stream.readFloat();

            track.AddKey(newKey);
        }
    }

    stream.seek(currentStreamPos);
}

void J3DAnimation::J3DJointFullAnimationInstance::ReadRotationComponentTrack(
    bStream::CStream& stream,
    J3DDiscreteAnimationTrack& track,
    uint32_t valueTableOffset, float scale
) {
    uint16_t keyCount = stream.readUInt16();
    uint16_t firstKeyIndex = stream.readUInt16();

    size_t currentStreamPos = stream.tell();
    stream.seek(valueTableOffset + firstKeyIndex * sizeof(uint16_t));

    if (keyCount == 1) {
        J3DAnimationKey newKey;
        newKey.Value = static_cast<float>(stream.readInt16()) * scale;

        track.AddKey(newKey);
    }
    else {
        for (uint16_t i = 0; i < keyCount; i++) {
            J3DAnimationKey newKey;

            //newKey.Time = static_cast<float>(stream.readInt16());
            newKey.Value = static_cast<float>(stream.readInt16()) * scale;

            track.AddKey(newKey);
        }
    }

    stream.seek(currentStreamPos);
}

void J3DAnimation::J3DJointFullAnimationInstance::Deserialize(bStream::CStream& stream) {
    size_t currentStreamPos = stream.tell();

    // Deserialize counts and offsets from the block
    J3DJointFullBlock jointFullBlock;
    jointFullBlock.Deserialize(&stream);

    mLength = jointFullBlock.Length;
    mLoopMode = static_cast<ELoopMode>(jointFullBlock.LoopMode);

    size_t fullTrackCount = jointFullBlock.TrackCount;

    float rotationScale = (180.0f / 32768.0f);

    stream.seek(jointFullBlock.TrackTableOffset);
    for (int i = 0; i < fullTrackCount; i++) {
        J3DJointFullAnimationData animData;
        animData.JointIndex = i;

        ReadFloatComponentTrack(stream, animData.ScaleX, jointFullBlock.ScaleTableOffset);
        ReadRotationComponentTrack(stream, animData.RotationX, jointFullBlock.RotationTableOffset, rotationScale);
        ReadFloatComponentTrack(stream, animData.TranslationX, jointFullBlock.TranslationTableOffset);

        ReadFloatComponentTrack(stream, animData.ScaleY, jointFullBlock.ScaleTableOffset);
        ReadRotationComponentTrack(stream, animData.RotationY, jointFullBlock.RotationTableOffset, rotationScale);
        ReadFloatComponentTrack(stream, animData.TranslationY, jointFullBlock.TranslationTableOffset);

        ReadFloatComponentTrack(stream, animData.ScaleZ, jointFullBlock.ScaleTableOffset);
        ReadRotationComponentTrack(stream, animData.RotationZ, jointFullBlock.RotationTableOffset, rotationScale);
        ReadFloatComponentTrack(stream, animData.TranslationZ, jointFullBlock.TranslationTableOffset);

        mEntries.push_back(animData);
    }

    stream.seek(currentStreamPos + jointFullBlock.BlockSize);
}

std::vector<glm::mat4> J3DAnimation::J3DJointFullAnimationInstance::GetTransformsAtFrame(float deltaTime) {
    std::vector<glm::mat4> transforms;
    float frameTime = GetFrame();

    for (const J3DJointFullAnimationData& j : mEntries) {
        glm::vec3 translation = glm::vec3(j.TranslationX.GetValue(frameTime), j.TranslationY.GetValue(frameTime), j.TranslationZ.GetValue(frameTime));
        glm::vec3 scale = glm::vec3(j.ScaleX.GetValue(frameTime), j.ScaleY.GetValue(frameTime), j.ScaleZ.GetValue(frameTime));
        glm::vec3 eulerRotation = glm::vec3(
            glm::radians(j.RotationX.GetValue(frameTime)),
            glm::radians(j.RotationY.GetValue(frameTime)),
            glm::radians(j.RotationZ.GetValue(frameTime))
        );

        glm::quat rotation = glm::angleAxis(eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::angleAxis(eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::angleAxis(eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));


        transforms.push_back(glm::translate(translation) * glm::toMat4(rotation) * glm::scale(scale));
    }

    return transforms;
}