#pragma once

#include "J3D/Rendering/J3DRenderPacket.hpp"
#include "J3D/Rendering/J3DLight.hpp"
#include "J3D/Util/J3DTransform.hpp"
#include "J3D/Util/J3DUtil.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace J3DAnimation {
    class J3DColorAnimationInstance;
    class J3DTexIndexAnimationInstance;
    class J3DTexMatrixAnimationInstance;
    class J3DJointAnimationInstance;
    class J3DJointFullAnimationInstance;
    class J3DVisibilityAnimationInstance;
}

struct J3DTexture;

class J3DMaterial;
class J3DModelData;
class J3DMaterialTable;

class J3DModelInstance {
    std::shared_ptr<J3DModelData> mModelData;
    std::vector<glm::mat4> mEnvelopeMatrices;
    J3DTransformInfo mTransform;

    glm::vec3 mBBMin;
    glm::vec3 mBBMax;

    // Transform applied to the model-space transform stored in mTransform.
    glm::mat4 mReferenceFrame;

    // Lights applied to this model instance
    J3DLight mLights[8];

    // Recalculates joint transforms based on a load animation - BCK for keyframes at discrete time units, BCA for values at every frame.
    void CalculateJointMatrices(float deltaTime);
    // Recalculates texture transforms based on a loaded BTK animation.
    void UpdateMaterialTextureMatrices(float deltaTime, std::shared_ptr<J3DMaterial> material, glm::mat4& viewMatrix, glm::mat4& projMatrix);

    // Updates material textures based on a loaded BTP animation.
    void UpdateMaterialTextures(float deltaTime, std::shared_ptr<J3DMaterial> material);

    // Updates material colors based on a loaded BPK animation.
    void UpdateMaterialColors(float deltaTime);
    // Updates TEV register colors based on a loaded BRK animation.
    void UpdateTEVRegisterColors(float deltaTime, std::shared_ptr<J3DMaterial> material);

    // Updates shape visibility based on a loaded BVA animation.
    void UpdateShapeVisibility(float deltaTime);

    void Update(float deltaTime, std::shared_ptr<J3DMaterial> material, glm::mat4& viewMatrix, glm::mat4& projMatrix);

    std::shared_ptr<J3DAnimation::J3DColorAnimationInstance> mRegisterColorAnimation;
    std::shared_ptr<J3DAnimation::J3DTexIndexAnimationInstance> mTexIndexAnimation;
    std::shared_ptr<J3DAnimation::J3DTexMatrixAnimationInstance> mTexMatrixAnimation;
    std::shared_ptr<J3DAnimation::J3DJointAnimationInstance> mJointAnimation;
    std::shared_ptr<J3DAnimation::J3DJointFullAnimationInstance> mJointFullAnimation;
    std::shared_ptr<J3DAnimation::J3DVisibilityAnimationInstance> mVisibilityAnimation;

    bool bUseInstanceMaterialTable;
    std::shared_ptr<J3DMaterialTable> mInstanceMaterialTable;

    uint8_t mSortBias;
    uint16_t mModelId;

    bool CheckUseInstanceMaterials();
    bool CheckUseInstanceTextures();

public:
    J3DModelInstance(std::shared_ptr<J3DModelData> modelData, uint16_t id);
    virtual ~J3DModelInstance();

    void SetTranslation(const glm::vec3 trans);
    void SetRotation(const glm::vec3 rot);
    void SetScale(const glm::vec3 scale);

    void SetTransform(const glm::mat4 transform);
    void SetReferenceFrame(const glm::mat4 frame);

    void GetBoundingBox(glm::vec3& min, glm::vec3& max) const;

    void GatherRenderPackets(std::vector<J3DRenderPacket>& packetList, glm::vec3 cameraPosition);

    void UpdateAnimations(float deltaTime);
    void Render(float deltaTime, std::shared_ptr<J3DMaterial> material, glm::mat4& viewMatrix, glm::mat4& projMatrix, uint32_t materialShaderOverride = 0);

    J3DLight GetLight(int index) const;
    void SetLight(const J3DLight& light, int index);

    std::shared_ptr<J3DAnimation::J3DColorAnimationInstance> GetRegisterColorAnimation() const { return mRegisterColorAnimation; }
    void SetRegisterColorAnimation(std::shared_ptr<J3DAnimation::J3DColorAnimationInstance> anim) { mRegisterColorAnimation = anim; }

    std::shared_ptr<J3DAnimation::J3DTexIndexAnimationInstance> GetTexIndexAnimation() const { return mTexIndexAnimation; }
    void SetTexIndexAnimation(std::shared_ptr<J3DAnimation::J3DTexIndexAnimationInstance> anim) { mTexIndexAnimation = anim; }

    std::shared_ptr<J3DAnimation::J3DTexMatrixAnimationInstance> GetTexMatrixAnimation() const { return mTexMatrixAnimation; }
    void SetTexMatrixAnimation(std::shared_ptr<J3DAnimation::J3DTexMatrixAnimationInstance> anim) { mTexMatrixAnimation = anim; }

    std::shared_ptr<J3DAnimation::J3DJointAnimationInstance> GetJointAnimation() const { return mJointAnimation; }
    void SetJointAnimation(std::shared_ptr<J3DAnimation::J3DJointAnimationInstance> anim);

    std::shared_ptr<J3DAnimation::J3DJointFullAnimationInstance> GetJointFullAnimation() const { return mJointFullAnimation; }
    void SetJointFullAnimation(std::shared_ptr<J3DAnimation::J3DJointFullAnimationInstance> anim);

    std::shared_ptr<J3DAnimation::J3DVisibilityAnimationInstance> GetVisibilityAnimation() const { return mVisibilityAnimation; }
    void SetVisibilityAnimation(std::shared_ptr<J3DAnimation::J3DVisibilityAnimationInstance> anim) { mVisibilityAnimation = anim; }

    bool GetUseInstanceMaterialTable() const { return bUseInstanceMaterialTable; }
    void SetUseInstanceMaterialTable(bool use) { bUseInstanceMaterialTable = use; }

    void SetInstanceMaterialTable(std::shared_ptr<J3DMaterialTable> matTable) { mInstanceMaterialTable = matTable; }

    // Sets a bias value that affects a model's position in the rendering list.
    // The higher the bias value, the earlier the model will be rendered.
    void SetSortBias(uint8_t bias) { mSortBias = bias; }
    uint8_t GetSortBias() const { return mSortBias; }

    // Returns this model's unique ID.
    uint16_t GetModelId() const { return mModelId; }
};
