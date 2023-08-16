#include "J3D/J3DRenderPacket.hpp"
#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DModelInstance.hpp"

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
