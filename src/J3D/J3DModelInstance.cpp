#include "J3D/J3DModelInstance.hpp"
#include "J3D/J3DModelData.hpp"
#include "J3D/J3DUniformBufferObject.hpp"

#include <stdexcept>
#include <iostream>

J3DModelInstance::J3DModelInstance(std::shared_ptr<J3DModelData> modelData) {
    if (modelData == nullptr)
        throw std::invalid_argument("Tried to create a J3DModelInstance from invalid J3DModelData pointer!");

    mModelData = modelData;
    mEnvelopeMatrices = mModelData->GetRestPose();
}

J3DModelInstance::~J3DModelInstance() {

}

void J3DModelInstance::CalculateJointMatrices(float deltaTime) {
    // TODO: implement BCK/BCA
}

void J3DModelInstance::CalculateTextureMatrices(float deltaTime) {
    // TODO: implement BTK
}

void J3DModelInstance::UpdateMaterialTextures(float deltaTime) {
    // TODO: implement BTP
}

void J3DModelInstance::UpdateMaterialColors(float deltaTime) {
    // TODO: implement BPK
}

void J3DModelInstance::UpdateTEVRegisterColors(float deltaTime) {
    // TODO: implement BRK
}

void J3DModelInstance::UpdateShapeVisibility(float deltaTime) {
    // TODO: implement BVA
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

void J3DModelInstance::GatherRenderPackets(std::vector<J3DRenderPacket>& packetList, glm::vec3 cameraPosition) {
    glm::mat4 transformMat4 = mTransform.ToMat4();

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

        packetList.push_back({ sortKey, transformMat4, mEnvelopeMatrices, mat, mModelData });
    }
}

void J3DModelInstance::Render(float deltaTime) {
    J3DUniformBufferObject::SetEnvelopeMatrices(mEnvelopeMatrices.data(), mEnvelopeMatrices.size());
    
    glm::mat4 transformMat4 = mTransform.ToMat4();
    J3DUniformBufferObject::SetModelMatrix(&transformMat4);

    mModelData->Render(deltaTime);
}
