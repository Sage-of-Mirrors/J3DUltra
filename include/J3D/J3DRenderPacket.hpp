#pragma once

#include <glm/glm.hpp>

#include <stdint.h>
#include <memory>
#include <vector>

struct J3DRenderPacket {
    uint64_t SortKey;
    glm::mat4 ModelMatrix;
    std::vector<glm::mat4> EnvelopeMatrices;
    std::weak_ptr<class J3DMaterial> Material;
    std::weak_ptr<class J3DModelData> Data;

    void Render(float deltaTime);
};
