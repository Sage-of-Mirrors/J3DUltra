#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <functional>
#include <memory>

struct J3DRenderPacket;
class J3DModelInstance;

namespace J3D {
    namespace Rendering {
        using RenderPacketVector = std::vector<J3DRenderPacket>;
        using ModelInstanceVector = const std::vector<std::shared_ptr<J3DModelInstance>>&;

        void SetSortFunction(std::function<void(RenderPacketVector&)> sortFunction);
        RenderPacketVector SortPackets(ModelInstanceVector& modelInstances, glm::vec3 cameraPosition);

        void Render(float deltaTime, glm::mat4& viewMatrix, glm::mat4& projMatrix,
                    RenderPacketVector& modelInstances, uint32_t materialShaderOverride = 0);

        // Call this after Render to reuse the model calculations view/proj matrices for static rendering.
        // Note: This is used by J3D::Picking
        void StaticRender(RenderPacketVector& modelInstances, uint32_t materialShaderOverride = 0);
    }
}
