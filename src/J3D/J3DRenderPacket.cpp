#include "J3D/J3DRenderPacket.hpp"
#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DModelData.hpp"
#include "J3D/J3DUniformBufferObject.hpp"

#include <iostream>

void J3DRenderPacket::Render(float deltaTime)
{
    if (Material.expired() || Data.expired())
    {
        std::cout << "Material or data pointers were expired!" << std::endl;
        return;
    }

    std::shared_ptr<J3DMaterial> materialLocked = Material.lock();
    std::shared_ptr<J3DModelData> dataLocked = Data.lock();

    J3DUniformBufferObject::SetModelMatrix(&ModelMatrix);
    J3DUniformBufferObject::SetEnvelopeMatrices(EnvelopeMatrices.data(), EnvelopeMatrices.size());

    std::vector<std::shared_ptr<J3DTexture>> textures = dataLocked->GetTextures();

    dataLocked->BindVAO();
    materialLocked->Render(textures);
    dataLocked->UnbindVAO();
}
