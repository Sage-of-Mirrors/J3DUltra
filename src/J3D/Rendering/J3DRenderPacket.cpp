#include "J3D/Rendering/J3DRenderPacket.hpp"
#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Data/J3DModelInstance.hpp"

#include <iostream>

void J3DRenderPacket::Render(float deltaTime, glm::mat4& viewMatrix, glm::mat4& projMatrix)
{
    if (Material == nullptr || Instance == nullptr)
    {
        std::cout << "Material or data pointers were invalid!" << std::endl;
        return;
    }

    Instance->Render(deltaTime, Material, viewMatrix, projMatrix);
}
