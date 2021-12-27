#include "J3D/J3DData.hpp"
#include "../lib/bStream/bstream.h"

bool J3DDataBase::Deserialize(bStream::CStream* stream) {
	try {
		J3DVersion = (EJ3DVersion)stream->readUInt32();
		FormatVersion = (EJ3DFormatVersion)stream->readUInt32();
		Size = stream->readUInt32();
		BlockCount = stream->readUInt32();

		stream->readBytesTo(Watermark, 16);
	}
	catch (...) {
		return false;
	}

	return true;
}
