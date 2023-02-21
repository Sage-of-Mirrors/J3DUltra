#pragma once

#include <string>
#include <vector>

namespace bStream { class CStream; }

class J3DNameTable {
	std::vector<std::string> mNames;

	uint16_t HashName(std::string name);

public:
	J3DNameTable() {}
	~J3DNameTable() {}

	void Serialize(bStream::CStream* stream);
	void Deserialize(bStream::CStream* stream);

	std::string GetName(uint16_t index) const;
	void AddName(std::string name);
};
