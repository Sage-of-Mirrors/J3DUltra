#pragma once

#include <glm/glm.hpp>

#include <stdint.h>
#include <memory>
#include <vector>

struct J3DRenderPacket {
    uint64_t SortKey;

    std::shared_ptr<class J3DMaterial> Material;
    class J3DModelInstance* Instance;

    void Render(float deltaTime);
};
