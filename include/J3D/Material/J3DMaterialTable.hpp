#pragma once

#include "J3D/Util/J3DUtil.hpp"

#include <string>
#include <vector>

class J3DMaterial;
struct J3DTexture;

class J3DMaterialTable {
    friend class J3DMaterialTableLoader;
    friend class J3DModelLoader;

    shared_vector<J3DMaterial> mMaterials;
    shared_vector<J3DTexture> mTextures;

public:
    J3DMaterialTable();
    ~J3DMaterialTable();

    std::shared_ptr<J3DMaterial> GetMaterial(uint32_t idx);
    std::shared_ptr<J3DMaterial> GetMaterial(std::string name);
    shared_vector<J3DMaterial>& GetMaterials() { return mMaterials; }

    std::shared_ptr<J3DTexture> GetTexture(uint32_t idx);
    std::shared_ptr<J3DTexture> GetTexture(std::string name);
    shared_vector<J3DTexture>& GetTextures() { return mTextures; }

    bool SetTexture(uint32_t idx, uint32_t width, uint32_t height, uint8_t* data, uint32_t size);
    bool SetTexture(std::string name, uint32_t width, uint32_t height, uint8_t* data, uint32_t size);
};
