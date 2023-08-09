#include "J3D/J3DRenderPacket.hpp"
#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DModelInstance.hpp"

#include <iostream>

void J3DRenderPacket::Render(float deltaTime)
{
    if (Material == nullptr || Instance == nullptr)
    {
        std::cout << "Material or data pointers were invalid!" << std::endl;
        return;
    }

    Instance->Render(deltaTime, Material);
}
