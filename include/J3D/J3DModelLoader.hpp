#pragma once

#include "J3DBlock.hpp"

#include <cstdint>
#include <memory>

namespace bStream { class CStream; }
class J3DModelData;
class J3DJoint;

constexpr uint32_t FLAGS_MATRIX_MASK = 0x0000000F;

class J3DModelLoader {
	std::shared_ptr<J3DModelData> mModelData;

public:
	J3DModelLoader();
	virtual ~J3DModelLoader() {}

	virtual std::shared_ptr<J3DModelData> Load(bStream::CStream* stream, uint32_t flags);

protected:

	void ReadInformationBlock(bStream::CStream* stream, uint32_t flags);
	void ReadVertexBlock(bStream::CStream* stream, uint32_t flags);
	void ReadEnvelopeBlock(bStream::CStream* stream, uint32_t flags);
	void ReadDrawBlock(bStream::CStream* stream, uint32_t flags);
	void ReadJointBlock(bStream::CStream* stream, uint32_t flags);
	void ReadShapeBlock(bStream::CStream* stream, uint32_t flags);
	void ReadMaterialBlock(bStream::CStream* stream, uint32_t flags);
	void ReadTextureBlock(bStream::CStream* stream, uint32_t flags);
};
