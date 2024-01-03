#include "J3D/Rendering/J3DRendering.hpp"
#include "J3D/Rendering/J3DRenderPacket.hpp"
#include "J3D/Data/J3DModelInstance.hpp"

namespace J3DRendering {
    namespace {
        std::function<void(SortFunctionArgs)> SortFunction = [](SortFunctionArgs) {};
    }
}

void J3DRendering::SetSortFunction(std::function<void(SortFunctionArgs)> sortFunction) {
    if (sortFunction) {
        SortFunction = sortFunction;
    }
}

void J3DRendering::Render(float deltaTime, glm::vec3 cameraPosition, glm::mat4& viewMatrix, glm::mat4& projMatrix, RenderFunctionArgs modelInstances, uint32_t materialShaderOverride) {
    std::vector<J3DRenderPacket> packets;

    for (std::shared_ptr<J3DModelInstance> instance : modelInstances) {
        instance->GatherRenderPackets(packets, cameraPosition);
    }

    SortFunction(packets);

    for (J3DRenderPacket packet : packets) {
        packet.Render(deltaTime, viewMatrix, projMatrix, materialShaderOverride);
    }
}
