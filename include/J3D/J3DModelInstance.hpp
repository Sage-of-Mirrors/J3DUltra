#pragma once

#include "J3D/J3DTransform.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class J3DModelData;

class J3DModelInstance {
    std::shared_ptr<J3DModelData> mModelData;
    std::vector<glm::mat4> mEnvelopeMatrices;
    J3DTransformInfo mTransform;

    void CalculateJointMatrices(float deltaTime);

public:
    J3DModelInstance(std::shared_ptr<J3DModelData> modelData);

    void SetTranslation(const glm::vec3 trans);
    void SetRotation(const glm::vec3 rot);
    void SetScale(const glm::vec3 scale);

    void Render(float deltaTime);
};
