#include "J3D/Animation/J3DTexMatrixAnimationInstance.hpp"

#include "J3D/J3DModelInstance.hpp"
#include "J3D/J3DNameTable.hpp"
#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DMaterialData.hpp"

#include <memory>
#include <algorithm>

J3DAnimation::J3DTexMatrixAnimationInstance::J3DTexMatrixAnimationInstance() {

}

void J3DAnimation::J3DTexMatrixAnimationInstance::ReadFloatComponentTrack(bStream::CStream& stream, J3DHermiteAnimationTrack& track, uint32_t valueTableOffset) {
    uint16_t keyCount = stream.readUInt16();
    uint16_t firstKeyIndex = stream.readUInt16();
    ETangentMode tangentMode = static_cast<ETangentMode>(stream.readUInt16());

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

            switch (tangentMode) {
            case ETangentMode::Symmetric:
                newKey.Time = stream.readFloat();
                newKey.Value = stream.readFloat();
                newKey.InTangent = stream.readFloat();
                newKey.OutTangent = newKey.InTangent;
                break;
            case ETangentMode::Piecewise:
                newKey.Time = stream.readFloat();
                newKey.Value = stream.readFloat();
                newKey.InTangent = stream.readFloat();
                newKey.OutTangent = stream.readFloat();
                break;
            default:
                break;
            }

            track.AddKey(newKey);
        }
    }

    stream.seek(currentStreamPos);
}

void J3DAnimation::J3DTexMatrixAnimationInstance::ReadRotationComponentTrack(
    bStream::CStream& stream,
    J3DHermiteAnimationTrack& track,
    uint32_t valueTableOffset, float scale
) {
    uint16_t keyCount = stream.readUInt16();
    uint16_t firstKeyIndex = stream.readUInt16();
    ETangentMode tangentMode = static_cast<ETangentMode>(stream.readUInt16());

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

            switch (tangentMode) {
            case ETangentMode::Symmetric:
                newKey.Time = static_cast<float>(stream.readInt16());
                newKey.Value = static_cast<float>(stream.readInt16()) * scale;
                newKey.InTangent = static_cast<float>(stream.readInt16()) * scale;
                newKey.OutTangent = newKey.InTangent;
                break;
            case ETangentMode::Piecewise:
                newKey.Time = static_cast<float>(stream.readInt16());
                newKey.Value = static_cast<float>(stream.readInt16()) * scale;
                newKey.InTangent = static_cast<float>(stream.readInt16()) * scale;
                newKey.OutTangent = static_cast<float>(stream.readInt16()) * scale;
                break;
            default:
                break;
            }

            track.AddKey(newKey);
        }
    }

    stream.seek(currentStreamPos);
}

void J3DAnimation::J3DTexMatrixAnimationInstance::Deserialize(bStream::CStream& stream) {
    size_t currentStreamPos = stream.tell();

    // Deserialize counts and offsets from the block
    J3DTexMatrixKeyBlock matrixKeyBlock;
    matrixKeyBlock.Deserialize(&stream);

    mLength = matrixKeyBlock.Length;
    mLoopMode = static_cast<ELoopMode>(matrixKeyBlock.LoopMode);

    size_t fullTrackCount = matrixKeyBlock.TrackCount / 3;

    // Deserialize the material names
    stream.seek(matrixKeyBlock.MaterialNameTableOffset);
    J3DNameTable materialNames;
    materialNames.Deserialize(&stream);

    std::vector<uint8_t> texGenIndices;
    texGenIndices.reserve(matrixKeyBlock.TrackCount);

    // Deserialize tex gen index table
    stream.seek(matrixKeyBlock.TexGenIndexTableOffset);
    for (int i = 0; i < fullTrackCount; i++) {
        texGenIndices.push_back(stream.readUInt8());
    }

    std::vector<glm::vec3> matrixOrigins;
    matrixOrigins.reserve(matrixKeyBlock.TrackCount);

    // Deserialize matrix origin table
    stream.seek(matrixKeyBlock.TexMatrixOriginTableOffset);
    for (int i = 0; i < fullTrackCount; i++) {
        matrixOrigins.push_back({ stream.readFloat(), stream.readFloat(), stream.readFloat() });
    }

    float rotationScale = (1 << matrixKeyBlock.RotationFraction) * (180.0f / 32768.0f);

    stream.seek(matrixKeyBlock.TrackTableOffset);
    for (int i = 0; i < fullTrackCount; i++) {
        J3DTexMatrixAnimationData animData;
        animData.MaterialName = materialNames.GetName(i);
        animData.Origin = matrixOrigins[i];
        animData.TexGenIndex = texGenIndices[i];

        ReadFloatComponentTrack(stream, animData.ScaleS, matrixKeyBlock.ScaleTableOffset);
        ReadRotationComponentTrack(stream, animData.RotationS, matrixKeyBlock.RotationTableOffset, rotationScale);
        ReadFloatComponentTrack(stream, animData.TranslationS, matrixKeyBlock.TranslationTableOffset);

        ReadFloatComponentTrack(stream, animData.ScaleT, matrixKeyBlock.ScaleTableOffset);
        ReadRotationComponentTrack(stream, animData.RotationT, matrixKeyBlock.RotationTableOffset, rotationScale);
        ReadFloatComponentTrack(stream, animData.TranslationT, matrixKeyBlock.TranslationTableOffset);

        ReadFloatComponentTrack(stream, animData.ScaleQ, matrixKeyBlock.ScaleTableOffset);
        ReadRotationComponentTrack(stream, animData.RotationQ, matrixKeyBlock.RotationTableOffset, rotationScale);
        ReadFloatComponentTrack(stream, animData.TranslationQ, matrixKeyBlock.TranslationTableOffset);

        mEntries.push_back(animData);
    }

    stream.seek(currentStreamPos + matrixKeyBlock.BlockSize);
}

void J3DAnimation::J3DTexMatrixAnimationInstance::ApplyAnimation(std::shared_ptr<J3DMaterial> material) {
    auto texMatrices = material->TexGenBlock.mTexMatrix;

    for (int i = 0; i < texMatrices.size(); i++) {
        material->AnimationTexMatrixInfo[i] = *texMatrices[i];
    }

    float frameTime = GetFrame();

    for (auto t : mEntries) {
        J3DTexMatrixInfo& texMat = material->AnimationTexMatrixInfo[t.TexGenIndex];

        texMat.Origin = t.Origin;

        texMat.Transform.Translation = glm::vec3(
            t.TranslationS.GetValue(frameTime),
            t.TranslationT.GetValue(frameTime),
            t.TranslationQ.GetValue(frameTime)
        );
        texMat.Transform.Scale = glm::vec3(
            t.ScaleS.GetValue(frameTime),
            t.ScaleT.GetValue(frameTime),
            t.ScaleQ.GetValue(frameTime)
        );
        //texMat.Transform.Rotation = glm::vec3(
        //);
    }
}