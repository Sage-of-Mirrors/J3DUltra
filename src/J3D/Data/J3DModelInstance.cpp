#include "J3D/Data/J3DModelInstance.hpp"
#include "J3D/Data/J3DModelData.hpp"
#include "J3D/Material/J3DUniformBufferObject.hpp"
#include "J3D/Material/J3DMaterialTable.hpp"

#include "J3D/Animation/J3DColorAnimationInstance.hpp"
#include "J3D/Animation/J3DTexIndexAnimationInstance.hpp"
#include "J3D/Animation/J3DTexMatrixAnimationInstance.hpp"
#include "J3D/Animation/J3DJointAnimationInstance.hpp"
#include "J3D/Animation/J3DJointFullAnimationInstance.hpp"
#include "J3D/Animation/J3DVisibilityAnimationInstance.hpp"

#include "J3D/Skeleton/J3DJoint.hpp"

#include <stdexcept>
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

J3DModelInstance::J3DModelInstance(std::shared_ptr<J3DModelData> modelData) {
    if (modelData == nullptr)
        throw std::invalid_argument("Tried to create a J3DModelInstance from invalid J3DModelData pointer!");

    mModelData = modelData;
    mEnvelopeMatrices = mModelData->GetRestPose();
    mReferenceFrame = glm::identity<glm::mat4>();
    mSortBias = 0;
}

J3DModelInstance::~J3DModelInstance() {

}

void J3DModelInstance::CalculateJointMatrices(float deltaTime) {
    if (mJointAnimation == nullptr && mJointFullAnimation == nullptr) {
        return;
    }

    std::vector<glm::mat4> animTransforms;

    if (mJointAnimation != nullptr) {
        animTransforms = mJointAnimation->GetTransformsAtFrame(deltaTime);
    }
    else if (mJointFullAnimation != nullptr) {
        animTransforms = mJointFullAnimation->GetTransformsAtFrame(deltaTime);
    }
    else {
        animTransforms = { glm::identity<glm::mat4>() };
    }

    std::vector<glm::mat4> t;

    for (std::shared_ptr<J3DJoint> jnt : mModelData->GetJoints()) {
        std::shared_ptr<J3DJoint> p = jnt;

        glm::mat4 completeTransform = glm::identity<glm::mat4>();

        while (p != nullptr) {
            glm::mat4 parentTransform = animTransforms[p->GetJointID()];

            if (p->GetAttachFlag() == 1) {
                glm::vec3 scale, translation, skew;
                glm::vec4 persp;
                glm::quat rotation;

                glm::decompose(parentTransform, scale, rotation, translation, skew, persp);
                parentTransform = glm::inverse(glm::scale(scale)) * parentTransform;
            }

            completeTransform = animTransforms[p->GetJointID()] * completeTransform;
            p = std::dynamic_pointer_cast<J3DJoint>(p->GetParent());
        }

        t.push_back(completeTransform);
    }

    mEnvelopeMatrices = mModelData->CalculateAnimJointPose(t);
}

void J3DModelInstance::UpdateMaterialTextureMatrices(float deltaTime, std::shared_ptr<J3DMaterial> material, glm::mat4& viewMatrix, glm::mat4& projMatrix) {
    if (mTexMatrixAnimation != nullptr) {
        mTexMatrixAnimation->ApplyAnimation(material);
    }

    material->CalculateTexMatrices(mTransform.ToMat4(), viewMatrix, projMatrix);
}

void J3DModelInstance::UpdateMaterialTextures(float deltaTime, std::shared_ptr<J3DMaterial> material) {
    if (mTexIndexAnimation == nullptr) {
        return;
    }

    mTexIndexAnimation->ApplyAnimation(material);
}

void J3DModelInstance::UpdateMaterialColors(float deltaTime) {
    // TODO: implement BPK
}

void J3DModelInstance::UpdateTEVRegisterColors(float deltaTime, std::shared_ptr<J3DMaterial> material) {
    if (mRegisterColorAnimation == nullptr) {
        return;
    }

    mRegisterColorAnimation->ApplyAnimation(material);
}

void J3DModelInstance::UpdateShapeVisibility(float deltaTime) {
    if (mVisibilityAnimation == nullptr) {
        return;
    }

    const std::vector<GXShape*>& shapes = mModelData->GetShapes();
    for (uint32_t i = 0; i < shapes.size(); i++) {
        shapes[i]->SetVisible(mVisibilityAnimation->GetVisibilityAtFrame(i, deltaTime));
    }
}

void J3DModelInstance::Update(float deltaTime, std::shared_ptr<J3DMaterial> material, glm::mat4& viewMatrix, glm::mat4& projMatrix) {
    UpdateTEVRegisterColors(deltaTime, material);
    UpdateMaterialTextures(deltaTime, material);
    UpdateMaterialTextureMatrices(deltaTime, material, viewMatrix, projMatrix);
    UpdateShapeVisibility(deltaTime);
    CalculateJointMatrices(deltaTime);

    J3DUniformBufferObject::SetEnvelopeMatrices(mEnvelopeMatrices.data(), mEnvelopeMatrices.size());
    J3DUniformBufferObject::SetLights(mLights);

    glm::mat4 transformMat4 = mReferenceFrame * mTransform.ToMat4();
    J3DUniformBufferObject::SetModelMatrix(transformMat4);
}

void J3DModelInstance::SetTranslation(const glm::vec3 trans) {
    mTransform.Translation = trans;
}

void J3DModelInstance::SetRotation(const glm::vec3 rot) {
    glm::vec3 eulerRotation;
    eulerRotation.x = glm::radians(rot.x);
    eulerRotation.y = glm::radians(rot.y);
    eulerRotation.z = glm::radians(rot.z);

    mTransform.Rotation = glm::angleAxis(eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
                          glm::angleAxis(eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
                          glm::angleAxis(eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
}

void J3DModelInstance::SetScale(const glm::vec3 scale) {
    mTransform.Scale = scale;
}

void J3DModelInstance::SetTransform(const glm::mat4 transform) {
    glm::vec3 translation, scale, skew;
    glm::vec4 perspective;
    glm::quat rotation;

    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    mTransform.Translation = translation;
    mTransform.Scale = scale;
    mTransform.Rotation = rotation;
}

void J3DModelInstance::GetBoundingBox(glm::vec3& min, glm::vec3& max) const {
    mModelData->GetBoundingBox(min, max);
}

J3DLight J3DModelInstance::GetLight(int index) const {
    J3DLight light;
    
    if (index >= 0 && index < 8) {
        light = mLights[index];
    }

    return light;
}

void J3DModelInstance::SetLight(const J3DLight& light, int index) {
    if (index < 0 || index >= 8) {
        return;
    }

    mLights[index] = light;
}

void J3DModelInstance::SetReferenceFrame(const glm::mat4 frame) {
    mReferenceFrame = frame;
}

void J3DModelInstance::GatherRenderPackets(std::vector<J3DRenderPacket>& packetList, glm::vec3 cameraPosition) {
    glm::mat4 transformMat4 = mReferenceFrame * mTransform.ToMat4();

    shared_vector<J3DMaterial> materials = CheckUseInstanceMaterials() ? mInstanceMaterialTable->GetMaterials() : mModelData->GetMaterials();

    for (std::shared_ptr<J3DMaterial> mat : materials)
    {
        if (mat->GetShape() == nullptr) {
            continue;
        }

        const glm::vec3& center = mat->GetShape()->GetCenterOfMass();
        glm::vec4 transformedCenter = transformMat4 * glm::vec4(center.x, center.y, center.z, 1.0f);

        float distToCamera = glm::distance(cameraPosition, glm::vec3(transformedCenter.x, transformedCenter.y, transformedCenter.z));
        uint32_t sortKey = static_cast<uint32_t>(distToCamera) & 0x7FFFFF;

        if (mat->PEMode == EPixelEngineMode::Opaque || mat->PEMode == EPixelEngineMode::AlphaTest)
        {
            sortKey |= 0x00800000;
        }

        sortKey |= mSortBias << 24;

        packetList.push_back({ sortKey, mat, this });
    }
}

void J3DModelInstance::UpdateAnimations(float deltaTime) {
    if (mRegisterColorAnimation != nullptr) {
        mRegisterColorAnimation->Tick(deltaTime);
    }

    if (mTexIndexAnimation != nullptr) {
        mTexIndexAnimation->Tick(deltaTime);
    }

    if (mTexMatrixAnimation != nullptr) {
        mTexMatrixAnimation->Tick(deltaTime);
    }

    if (mJointAnimation != nullptr) {
        mJointAnimation->Tick(deltaTime);
    }

    if (mJointAnimation == nullptr && mJointFullAnimation != nullptr) {
        mJointFullAnimation->Tick(deltaTime);
    }

    if (mVisibilityAnimation != nullptr) {
        mVisibilityAnimation->Tick(deltaTime);
    }
}

void J3DModelInstance::Render(float deltaTime, std::shared_ptr<J3DMaterial> material, glm::mat4& viewMatrix, glm::mat4& projMatrix, uint32_t materialShaderOverride) {
    Update(deltaTime, material, viewMatrix, projMatrix);

    mModelData->BindVAO();

    auto& textures = CheckUseInstanceTextures() ? mInstanceMaterialTable->GetTextures() : mModelData->GetTextures();
    material->Render(textures, materialShaderOverride);

    mModelData->UnbindVAO();
}

bool J3DModelInstance::CheckUseInstanceMaterials() {
    return bUseInstanceMaterialTable && mInstanceMaterialTable != nullptr && mInstanceMaterialTable->GetMaterials().size() != 0;
}

bool J3DModelInstance::CheckUseInstanceTextures() {
    return bUseInstanceMaterialTable && mInstanceMaterialTable != nullptr && mInstanceMaterialTable->GetTextures().size() != 0;
}
