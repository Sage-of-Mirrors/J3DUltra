#pragma once

#include "J3DNameTable.hpp"

#include <vector>
#include <cstdint>

namespace bStream { class CStream; }
class J3DMaterial;
struct J3DMaterialBlock;

class J3DMaterialFactory {
	J3DMaterialBlock* mBlock;

	std::vector<uint16_t> mInstanceTable;
	J3DNameTable mNameTable;

public:
	J3DMaterialFactory(J3DMaterialBlock* srcBlock, bStream::CStream* stream);
	~J3DMaterialFactory() {}

	J3DMaterial* Create(bStream::CStream* stream, uint32_t index);
};
