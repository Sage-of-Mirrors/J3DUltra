#include "J3D/Animation/J3DColorAnimationInstance.hpp"

#include "J3D/J3DModelInstance.hpp"
#include "J3D/J3DNameTable.hpp"

#include <memory>

J3DAnimation::J3DColorAnimationInstance::J3DColorAnimationInstance() {

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

        animData.RedTrack.Deserialize(stream, colorKeyBlock.RegisterRedTableOffset, 255.0f);
        animData.GreenTrack.Deserialize(stream, colorKeyBlock.RegisterGreenTableOffset, 255.0f);
        animData.BlueTrack.Deserialize(stream, colorKeyBlock.RegisterBlueTableOffset, 255.0f);
        animData.AlphaTrack.Deserialize(stream, colorKeyBlock.RegisterAlphaTableOffset, 255.0f);

        animData.ColorIndex = stream.readUInt8();
        stream.skip(3);

        RegisterEntries.push_back(animData);
    }

    // Deserialize the tracks for konst colors
    stream.seek(colorKeyBlock.KonstTrackTableOffset);
    for (int i = 0; i < colorKeyBlock.KonstTrackCount; i++) {
        J3DColorAnimationData animData;
        animData.MaterialName = konstMaterialNames.GetName(i);

        animData.RedTrack.Deserialize(stream, colorKeyBlock.KonstRedTableOffset, 255.0f);
        animData.GreenTrack.Deserialize(stream, colorKeyBlock.KonstGreenTableOffset, 255.0f);
        animData.BlueTrack.Deserialize(stream, colorKeyBlock.KonstBlueTableOffset, 255.0f);
        animData.AlphaTrack.Deserialize(stream, colorKeyBlock.KonstAlphaTableOffset, 255.0f);

        animData.ColorIndex = stream.readUInt8();
        stream.skip(3);

        KonstEntries.push_back(animData);
    }

    stream.seek(currentStreamPos + colorKeyBlock.BlockSize);
}

void J3DAnimation::J3DColorAnimationInstance::ApplyAnimation(std::shared_ptr<J3DModelInstance> model) {

}
