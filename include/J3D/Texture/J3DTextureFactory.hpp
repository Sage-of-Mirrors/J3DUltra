#pragma once

#include "J3DTexture.hpp"
#include "J3D/Util/J3DNameTable.hpp"
#include "GX/GXEnum.hpp"

#include <vector>
#include <memory>
#include <cstdint>

namespace bStream { class CStream; }
struct J3DTextureBlock;

constexpr uint32_t TEXTURE_ENTRY_SIZE = 32;

class J3DTextureFactory {
	J3DTextureBlock* mBlock;
	J3DNameTable mNameTable;

public:
	J3DTextureFactory(J3DTextureBlock* srcBlock, bStream::CStream* stream);
	~J3DTextureFactory() {}

	std::shared_ptr<J3DTexture> Create(bStream::CStream* stream, uint32_t index);
};
