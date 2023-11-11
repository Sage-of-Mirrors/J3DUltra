#include "J3D/Util/J3DNameTable.hpp"
#include "J3D/Util/J3DUtil.hpp"

#include <bstream.h>

const uint16_t HEADER_SIZE = 4;
const uint16_t ENTRY_SIZE = 4;

uint16_t J3DNameTable::HashName(std::string name) {
	uint16_t hash = 0;

	for (char c : name) {
		hash = hash * 3 + c;
	}

	return hash;
}

void J3DNameTable::Serialize(bStream::CStream* stream) {
	stream->writeUInt16(mNames.size());
	stream->writeUInt16(UINT16_MAX);

	uint16_t runningOffset = HEADER_SIZE + mNames.size() * ENTRY_SIZE;

	for (std::string name : mNames) {
		stream->writeUInt16(HashName(name));
		stream->writeUInt16(runningOffset);

		runningOffset += name.length() + 1;
	}

	for (std::string name : mNames) {
		stream->writeString(name);
		stream->writeUInt8(0);
	}

	J3DUtility::PadStreamWithString(stream, 4);
}

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

std::string J3DNameTable::GetName(uint16_t index) const {
	if (index >= 0 && index < mNames.size())
		return mNames[index];

	return "";
}

void J3DNameTable::AddName(std::string name) {
	mNames.push_back(name);
}
