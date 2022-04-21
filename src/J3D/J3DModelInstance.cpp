#include "J3D/J3DModelInstance.hpp"
#include "J3D/J3DModelData.hpp"
#include "J3D/J3DUniformBufferObject.hpp"

#include <stdexcept>
#include <iostream>

J3DModelInstance::J3DModelInstance(std::shared_ptr<J3DModelData> modelData) {
    if (modelData == nullptr)
        throw std::invalid_argument("Tried to create a J3DModelInstance from invalid J3DModelData pointer!");

    mModelData = modelData;
}

void J3DModelInstance::CalculateJointMatrices(float deltaTime) {
    mEnvelopeMatrices = mModelData->GetRestPose();
}

void J3DModelInstance::SetTranslation(const glm::vec3 trans) {
    mTransform.Translation = trans;
}

void J3DModelInstance::SetRotation(const glm::vec3 rot) {

}

void J3DModelInstance::SetScale(const glm::vec3 scale) {
    mTransform.Scale = scale;
}

void J3DModelInstance::Render(float deltaTime) {
    CalculateJointMatrices(deltaTime);
    
    J3DUniformBufferObject::SetEnvelopeMatrices(mEnvelopeMatrices.data(), mEnvelopeMatrices.size());
    J3DUniformBufferObject::SetModelMatrix(&mTransform.ToMat4());

    mModelData->Render(deltaTime);
}
