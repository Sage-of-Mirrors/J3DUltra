#include "J3D/Animation/J3DColorAnimationInstance.hpp"

#include "J3D/J3DModelInstance.hpp"
#include "J3D/J3DNameTable.hpp"

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
