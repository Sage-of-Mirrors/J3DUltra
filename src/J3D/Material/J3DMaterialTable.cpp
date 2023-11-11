#pragma once

#include "J3D/Material/J3DMaterialTable.hpp"
#include "J3D/Material/J3DMaterial.hpp"

#include "J3D/Texture/J3DTexture.hpp"

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
    for (std::shared_ptr<J3DTexture>& t : mTextures) {
        if (t->Name == name) {
            return t;
        }
    }

    return std::shared_ptr<J3DTexture>();
}
