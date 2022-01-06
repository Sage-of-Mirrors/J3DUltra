#pragma once

#include <string>
#include <vector>

namespace bStream { class CStream; }

class J3DNameTable {
	std::vector<std::string> mNames;

public:
	J3DNameTable() {}
	~J3DNameTable() {}

	void Deserialize(bStream::CStream* stream);

	std::string GetName(uint16_t index);
};
