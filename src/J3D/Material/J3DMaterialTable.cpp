#pragma once

#include "J3D/Material/J3DMaterialTable.hpp"
#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Texture/J3DTexture.hpp"
#include "J3D/Texture/J3DTextureLoader.hpp"

#include "glad/glad.h"

J3DMaterialTable::J3DMaterialTable() {

}

J3DMaterialTable::~J3DMaterialTable() {

}

std::shared_ptr<J3DMaterial> J3DMaterialTable::GetMaterial(uint32_t idx) {
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

  J3DTextureLoader::InitTexture(tex);
  J3DTextureLoader::SetTextureMipImage(tex->TexHandle, 0, width, height, data);

  return true;
}

bool J3DMaterialTable::SetTexture(std::string name, uint32_t width, uint32_t height, uint8_t* data, uint32_t size) {
  // Since there may be multiple textures with the same name, set them all
  // (this is equivalent to behavior found in Super Mario Sunshine).
  bool all_good = true;
  for (uint32_t i = 0; i < mTextures.size(); i++) {
    if (mTextures[i]->Name == name) {
      all_good &= SetTexture(i, width, height, data, size);
    }
  }
  return all_good;
}
