#include "J3D/Data/J3DModelInstance.hpp"
#include "J3D/Data/J3DModelData.hpp"
#include "J3D/Material/J3DUniformBufferObject.hpp"

#include "J3D/Animation/J3DColorAnimationInstance.hpp"
#include "J3D/Animation/J3DTexIndexAnimationInstance.hpp"
#include "J3D/Animation/J3DTexMatrixAnimationInstance.hpp"

#include <stdexcept>
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>

J3DModelInstance::J3DModelInstance(std::shared_ptr<J3DModelData> modelData) {
    if (modelData == nullptr)
        throw std::invalid_argument("Tried to create a J3DModelInstance from invalid J3DModelData pointer!");

    mModelData = modelData;
    mEnvelopeMatrices = mModelData->GetRestPose();
    mReferenceFrame = glm::identity<glm::mat4>();
}

J3DModelInstance::~J3DModelInstance() {

}

void J3DModelInstance::CalculateJointMatrices(float deltaTime) {
    // TODO: implement BCK/BCA
}

void J3DModelInstance::UpdateMaterialTextureMatrices(float deltaTime, std::shared_ptr<J3DMaterial> material, glm::mat4& viewMatrix, glm::mat4& projMatrix) {
    if (mTexMatrixAnimation != nullptr) {
        mTexMatrixAnimation->ApplyAnimation(material);
    }

    material->CalculateTexMatrices(viewMatrix, projMatrix);
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
    // TODO: implement BVA
}

void J3DModelInstance::Update(float deltaTime, std::shared_ptr<J3DMaterial> material, glm::mat4& viewMatrix, glm::mat4& projMatrix) {
    UpdateTEVRegisterColors(deltaTime, material);
    UpdateMaterialTextures(deltaTime, material);
    UpdateMaterialTextureMatrices(deltaTime, material, viewMatrix, projMatrix);

    J3DUniformBufferObject::SetEnvelopeMatrices(mEnvelopeMatrices.data(), mEnvelopeMatrices.size());
    J3DUniformBufferObject::SetLights(mLights);

    glm::mat4 transformMat4 = mReferenceFrame * mTransform.ToMat4();
    J3DUniformBufferObject::SetModelMatrix(&transformMat4);
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

    for (std::shared_ptr<J3DMaterial> mat : mModelData->GetMaterials())
    {
        const glm::vec3& center = mat->GetShape()->GetCenterOfMass();
        glm::vec4 transformedCenter = transformMat4 * glm::vec4(center.x, center.y, center.z, 1.0f);

        float distToCamera = glm::distance(cameraPosition, glm::vec3(transformedCenter.x, transformedCenter.y, transformedCenter.z));
        uint32_t sortKey = static_cast<uint32_t>(distToCamera) & 0xFFFFFF;

        if (mat->PEMode == EPixelEngineMode::Opaque || mat->PEMode == EPixelEngineMode::AlphaTest)
        {
            sortKey |= 0x01000000;
        }

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
}

void J3DModelInstance::Render(float deltaTime, std::shared_ptr<J3DMaterial> material, glm::mat4& viewMatrix, glm::mat4& projMatrix) {
    Update(deltaTime, material, viewMatrix, projMatrix);

    mModelData->BindVAO();

    auto& textures = mModelData->GetTextures();
    material->Render(textures);

    mModelData->UnbindVAO();
}
