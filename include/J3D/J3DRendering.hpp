#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <memory>

struct J3DRenderPacket;
class J3DModelInstance;

namespace J3DRendering {
    using SortFunctionArgs = std::vector<J3DRenderPacket>&;
    using RenderFunctionArgs = const std::vector<std::shared_ptr<J3DModelInstance>>&;

    void SetSortFunction(std::function<void(SortFunctionArgs)> sortFunction);
    void Render(float deltaTime, glm::vec3 cameraPosition, RenderFunctionArgs modelInstances);
}