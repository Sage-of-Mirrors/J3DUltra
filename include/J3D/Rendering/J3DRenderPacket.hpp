#pragma once

#include <glm/glm.hpp>

#include <stdint.h>
#include <memory>
#include <vector>

struct J3DRenderPacket {
    uint64_t SortKey;

    std::shared_ptr<class J3DMaterial> Material;
    class J3DModelInstance* Instance;

    void Render(float deltaTime, glm::mat4& viewMatrix, glm::mat4& projMatrix, uint32_t materialShaderOverride = 0);

    // Call this after Render to reuse the model calculations and view/proj matrices for static rendering.
    void StaticRender(uint32_t materialShaderOverride = 0);
};
