#pragma once

#include "J3D/Material/J3DMaterialTable.hpp"
#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Texture/J3DTexture.hpp"
#include "J3D/Texture/J3DTextureFactory.hpp"

#include "glad/glad.h"

J3DMaterialTable::J3DMaterialTable() {

}

J3DMaterialTable::~J3DMaterialTable() {

}

std::shared_ptr<J3DMaterial> J3DMaterialTable::GetMaterial(uint32_t idx)  {
    return idx >= 0 && idx < mMaterials.size() ? mMaterials[idx] : std::shared_ptr<J3DMaterial>();
}

std::shared_ptr<J3DMaterial> J3DMaterialTable::GetMaterial(std::string name) {
    for (std::shared_ptr<J3DMaterial>& m : mMaterials) {
        if (m->Name == name) {
            return m;
        }
    }

    return std::shared_ptr<J3DMaterial>();
}

std::shared_ptr<J3DTexture> J3DMaterialTable::GetTexture(uint32_t idx) {
    return idx >= 0 && idx < mTextures.size() ? mTextures[idx] : std::shared_ptr<J3DTexture>();
}

std::shared_ptr<J3DTexture> J3DMaterialTable::GetTexture(std::string name) {
    for (std::shared_ptr<J3DTexture>& tex : mTextures) {
        if (tex->Name == name) {
            return tex;
        }
    }

    return std::shared_ptr<J3DTexture>();
}

bool J3DMaterialTable::SetTexture(uint32_t idx, uint32_t width, uint32_t height, uint8_t* data, uint32_t size) {
    if (idx >= mTextures.size()) {
        return false;
    }

    std::shared_ptr<J3DTexture> tex = mTextures[idx];
    tex->Width = width;
    tex->Height = height;

    // Disable mipmaps on this texture - we only support replacing textures with a single image.
    tex->MinFilter = EGXFilterMode::Linear;
    tex->MagFilter = EGXFilterMode::Linear;
    tex->MipmapsEnabled = false;
    tex->MipmapCount = 1;
    tex->MinLOD = 0;
    tex->MaxLOD = 0;
    tex->LODBias = 0;
    tex->MaxAnisotropy = EGXMaxAnisotropy::One;

    J3DTextureFactory::InitTexture(tex);
    J3DTextureFactory::SetTextureMipImage(tex->TexHandle, 0, width, height, data);

    return true;
}

bool J3DMaterialTable::SetTexture(std::string name, uint32_t width, uint32_t height, uint8_t* data, uint32_t size) {
    for (uint32_t i = 0; i < mTextures.size(); i++) {
        if (mTextures[i]->Name == name) {
            return SetTexture(i, width, height, data, size);
        }
    }

    return false;
}
