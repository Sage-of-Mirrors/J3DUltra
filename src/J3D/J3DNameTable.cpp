#include "J3D/J3DNameTable.hpp"
#include "../lib/bStream/bstream.h"

void J3DNameTable::Deserialize(bStream::CStream* stream) {
	uint32_t tableStartPos = stream->tell();

	uint16_t count = stream->readUInt16();
	stream->skip(2);

	for (int i = 0; i < count; i++) {
		stream->skip(2);
		uint16_t stringOffset = stream->readUInt16();

		char buffer[64];
		for (int i = 0; i < 64; i++) {
			buffer[i] = stream->peekUInt8(tableStartPos + stringOffset);
			if (buffer[i] == 0)
				break;

			stringOffset++;
		}

		mNames.push_back(std::string(buffer));
	}
}

std::string J3DNameTable::GetName(uint16_t index) {
	if (index >= 0 && index < mNames.size())
		return mNames[index];

	return "";
}
