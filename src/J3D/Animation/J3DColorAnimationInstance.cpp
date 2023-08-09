#include "J3D/Animation/J3DColorAnimationInstance.hpp"

#include "J3D/J3DModelInstance.hpp"
#include "J3D/J3DNameTable.hpp"
#include "J3D/J3DMaterial.hpp"

#include <memory>

J3DAnimation::J3DColorAnimationInstance::J3DColorAnimationInstance() {

}

void J3DAnimation::J3DColorAnimationInstance::ReadColorTrack(bStream::CStream& stream, J3DHermiteAnimationTrack& track, uint32_t valueTableOffset) {
    uint16_t keyCount = stream.readUInt16();
    uint16_t firstKeyIndex = stream.readUInt16();
    ETangentMode tangentMode = static_cast<ETangentMode>(stream.readUInt16());

    size_t currentStreamPos = stream.tell();
    stream.seek(valueTableOffset + firstKeyIndex * sizeof(uint16_t));

    if (keyCount == 1) {
        J3DAnimationKey newKey;
        newKey.Value = static_cast<float>(stream.readInt16());

        track.AddKey(newKey);
    }
    else {
        for (uint16_t i = 0; i < keyCount; i++) {
            J3DAnimationKey newKey;

            switch (tangentMode) {
                case ETangentMode::Symmetric:
                    newKey.Time = static_cast<float>(stream.readInt16());
                    newKey.Value = static_cast<float>(stream.readInt16());
                    newKey.InTangent = static_cast<float>(stream.readInt16()) / 65535.0f;
                    newKey.OutTangent = newKey.InTangent;
                    break;
                case ETangentMode::Piecewise:
                    newKey.Time = static_cast<float>(stream.readInt16());
                    newKey.Value = static_cast<float>(stream.readInt16());
                    newKey.InTangent = static_cast<float>(stream.readInt16()) / 65535.0f;
                    newKey.OutTangent = static_cast<float>(stream.readInt16()) / 65535.0f;
                    break;
                default:
                    break;
            }

            track.AddKey(newKey);
        }
    }

    stream.seek(currentStreamPos);
}

void J3DAnimation::J3DColorAnimationInstance::Deserialize(bStream::CStream& stream) {
    size_t currentStreamPos = stream.tell();

    // Deserialize counts and offsets from the block
    J3DRegisterColorKeyBlock colorKeyBlock;
    colorKeyBlock.Deserialize(&stream);

    mLength = colorKeyBlock.Length;
    mLoopMode = static_cast<ELoopMode>(colorKeyBlock.LoopMode);

    // Deserialize the material names for register colors
    stream.seek(colorKeyBlock.RegisterMaterialNameTableOffset);
    J3DNameTable regMaterialNames;
    regMaterialNames.Deserialize(&stream);

    // Deserialize the material names for konst colors
    stream.seek(colorKeyBlock.KonstMaterialNameTableOffset);
    J3DNameTable konstMaterialNames;
    konstMaterialNames.Deserialize(&stream);

    // Deserialize the tracks for register colors
    stream.seek(colorKeyBlock.RegisterTrackTableOffset);
    for (int i = 0; i < colorKeyBlock.RegisterTrackCount; i++) {
        J3DColorAnimationData animData;
        animData.MaterialName = regMaterialNames.GetName(i);

        ReadColorTrack(stream, animData.RedTrack, colorKeyBlock.RegisterRedTableOffset);
        ReadColorTrack(stream, animData.GreenTrack, colorKeyBlock.RegisterGreenTableOffset);
        ReadColorTrack(stream, animData.BlueTrack, colorKeyBlock.RegisterBlueTableOffset);
        ReadColorTrack(stream, animData.AlphaTrack, colorKeyBlock.RegisterAlphaTableOffset);

        animData.ColorIndex = stream.readUInt8();
        stream.skip(3);

        RegisterEntries.push_back(animData);
    }

    // Deserialize the tracks for konst colors
    stream.seek(colorKeyBlock.KonstTrackTableOffset);
    for (int i = 0; i < colorKeyBlock.KonstTrackCount; i++) {
        J3DColorAnimationData animData;
        animData.MaterialName = konstMaterialNames.GetName(i);

        ReadColorTrack(stream, animData.RedTrack, colorKeyBlock.KonstRedTableOffset);
        ReadColorTrack(stream, animData.GreenTrack, colorKeyBlock.KonstGreenTableOffset);
        ReadColorTrack(stream, animData.BlueTrack, colorKeyBlock.KonstBlueTableOffset);
        ReadColorTrack(stream, animData.AlphaTrack, colorKeyBlock.KonstAlphaTableOffset);

        animData.ColorIndex = stream.readUInt8();
        stream.skip(3);

        KonstEntries.push_back(animData);
    }

    stream.seek(currentStreamPos + colorKeyBlock.BlockSize);
}

void J3DAnimation::J3DColorAnimationInstance::ApplyAnimation(std::shared_ptr<J3DMaterial> material) {
    float frameTime = GetFrame();

    // Try to find register anim data for the current material.
    std::vector<J3DAnimation::J3DColorAnimationData>::const_iterator matFindResult = std::find_if(
        RegisterEntries.begin(),
        RegisterEntries.end(),
        [&material](const J3DAnimation::J3DColorAnimationData& v) { return v.MaterialName == material->Name; }
    );

    // Found register anim data! Now apply it.
    if (matFindResult != RegisterEntries.end()) {
        material->AreRegisterColorsAnimating = true;

        for (int i = 0; i < 4; i++) {
            material->AnimationRegisterColors[i] = material->TevBlock->mTevColors[i];
        }

        J3DAnimation::J3DColorAnimationData regData = *matFindResult;

        material->AnimationRegisterColors[regData.ColorIndex].r = regData.RedTrack.GetValue(frameTime);
        material->AnimationRegisterColors[regData.ColorIndex].g = regData.GreenTrack.GetValue(frameTime);
        material->AnimationRegisterColors[regData.ColorIndex].b = regData.BlueTrack.GetValue(frameTime);
        material->AnimationRegisterColors[regData.ColorIndex].a = regData.AlphaTrack.GetValue(frameTime);
    }

    // Try to find konst anim data for the current material.
    matFindResult = std::find_if(
        KonstEntries.begin(),
        KonstEntries.end(),
        [&material](const J3DAnimation::J3DColorAnimationData& v) { return v.MaterialName == material->Name; }
    );

    // Found konst anim data! Now apply it.
    if (matFindResult != KonstEntries.end()) {
        // Reset colors to default state before animating
        for (int i = 0; i < 4; i++) {
            material->AnimationKonstColors[i] = material->TevBlock->mTevKonstColors[i];
        }

        J3DAnimation::J3DColorAnimationData konstData = *matFindResult;

        material->AnimationKonstColors[konstData.ColorIndex].r = konstData.RedTrack.GetValue(frameTime);
        material->AnimationKonstColors[konstData.ColorIndex].g = konstData.GreenTrack.GetValue(frameTime);
        material->AnimationKonstColors[konstData.ColorIndex].b = konstData.BlueTrack.GetValue(frameTime);
        material->AnimationKonstColors[konstData.ColorIndex].a = konstData.AlphaTrack.GetValue(frameTime);
    }
}