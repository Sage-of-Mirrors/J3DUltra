#pragma once

#include "J3D/Util/J3DUtil.hpp"

#include <cstdint>
#include <vector>
#include <memory>

struct J3DTexture;

class J3DMaterial;
class J3DModelData;
class J3DMaterialTable;

class J3DMaterialTableLoader {
public:
	J3DMaterialTableLoader();
	~J3DMaterialTableLoader() {}

	std::shared_ptr<J3DMaterialTable> Load(bStream::CStream* stream, std::shared_ptr<J3DModelData> modelData);

protected:
	void ReadMaterialBlockV2(bStream::CStream* stream, std::shared_ptr<J3DMaterialTable> materialTable);
	void ReadMaterialBlockV3(bStream::CStream* stream, std::shared_ptr<J3DMaterialTable> materialTable);
	void ReadTextureBlock(bStream::CStream* stream, std::shared_ptr<J3DMaterialTable> materialTable);
};
