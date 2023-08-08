#pragma once

#include "J3D/J3DTransform.hpp"
#include "J3D/J3DRenderPacket.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace J3DAnimation {
    class J3DColorAnimationInstance;
}

class J3DModelData;

class J3DModelInstance {
    std::shared_ptr<J3DModelData> mModelData;
    std::vector<glm::mat4> mEnvelopeMatrices;
    J3DTransformInfo mTransform;

    // Recalculates joint transforms based on a load animation - BCK for keyframes at discrete time units, BCA for values at every frame.
    void CalculateJointMatrices(float deltaTime);
    // Recalculates texture transforms based on a loaded BTK animation.
    void CalculateTextureMatrices(float deltaTime);

    // Updates material textures based on a loaded BTP animation.
    void UpdateMaterialTextures(float deltaTime);

    // Updates material colors based on a loaded BPK animation.
    void UpdateMaterialColors(float deltaTime);
    // Updates TEV register colors based on a loaded BRK animation.
    void UpdateTEVRegisterColors(float deltaTime);

    // Updates shape visibility based on a loaded BVA animation.
    void UpdateShapeVisibility(float deltaTime);

    std::shared_ptr<J3DAnimation::J3DColorAnimationInstance> mRegisterColorAnimation;

public:
    J3DModelInstance(std::shared_ptr<J3DModelData> modelData);
    virtual ~J3DModelInstance();

    void SetTranslation(const glm::vec3 trans);
    void SetRotation(const glm::vec3 rot);
    void SetScale(const glm::vec3 scale);

    void SetTransform(const glm::mat4 transform);

    void GatherRenderPackets(std::vector<J3DRenderPacket>& packetList, glm::vec3 cameraPosition);

    void UpdateAnimations(float deltaTime);
    void Render(float deltaTime);

    void SetRegisterColorAnimation(std::shared_ptr<J3DAnimation::J3DColorAnimationInstance> anim) { mRegisterColorAnimation = anim; }
};
