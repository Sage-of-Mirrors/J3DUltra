#include "J3D/J3DBlock.hpp"
#include "../lib/bStream/bstream.h"

bool J3DBlock::Deserialize(bStream::CStream* stream) {
	try {
		BlockType = (EJ3DBlockType)stream->readUInt32();
		BlockSize = stream->readUInt32();
	}
	catch(...) {
		return false;
	}

	return true;
}

bool J3DModelInfoBlock::Deserialize(bStream::CStream* stream) {
	size_t currentStreamPosition = stream->tell();

	if (!J3DBlock::Deserialize(stream))
		return false;

	try {
		Flags = stream->readUInt16();
		MatrixGroupCount = stream->readUInt32();
		VertexPositionCount = stream->readUInt32();

		HierarchyOffset = stream->readUInt32() + currentStreamPosition;
	}
	catch (...) {
		return false;
	}

	return true;
}

bool J3DVertexBlock::Deserialize(bStream::CStream* stream) {
	size_t currentStreamPosition = stream->tell();

	if (!J3DBlock::Deserialize(stream))
		return false;

	try {
		AttributeTableOffset = stream->readUInt32() + currentStreamPosition;
		PositionTableOffset = stream->readUInt32() + currentStreamPosition;
		NormalTableOffset = stream->readUInt32() + currentStreamPosition;
		NBTTableOffset = stream->readUInt32() + currentStreamPosition;

		ColorTablesOffset[0] = stream->readUInt32() + currentStreamPosition;
		ColorTablesOffset[1] = stream->readUInt32() + currentStreamPosition;

		for (int i = 0; i < 8; i++) {
			TexCoordTablesOffset[i] = stream->readUInt32() + currentStreamPosition;
		}
	}
	catch (...) {
		return false;
	}

	return true;
}

bool J3DEnvelopeBlock::Deserialize(bStream::CStream* stream) {
	size_t currentStreamPosition = stream->tell();

	if (!J3DBlock::Deserialize(stream))
		return false;

	try {
		Count = stream->readUInt16();
		stream->skip(2);

		JointIndexTableOffset = stream->readUInt32() + currentStreamPosition;
		EnvelopeIndexTableOffset = stream->readUInt32() + currentStreamPosition;
		WeightTableOffset = stream->readUInt32() + currentStreamPosition;
		MatrixTableOffset = stream->readUInt32() + currentStreamPosition;
	}
	catch (...) {
		return false;
	}

	return true;
}

bool J3DDrawBlock::Deserialize(bStream::CStream* stream) {
	size_t currentStreamPosition = stream->tell();

	if (!J3DBlock::Deserialize(stream))
		return false;

	try {
		Count = stream->readUInt16();
		stream->skip(2);

		DrawTableOffset = stream->readUInt32() + currentStreamPosition;
		IndexTableOffset = stream->readUInt32() + currentStreamPosition;
	}
	catch (...) {
		return false;
	}

	return true;
}

bool J3DJointBlock::Deserialize(bStream::CStream* stream) {
	size_t currentStreamPosition = stream->tell();

	if (!J3DBlock::Deserialize(stream))
		return false;

	try {
		Count = stream->readUInt16();
		stream->skip(2);

		InitDataTableOffset = stream->readUInt32() + currentStreamPosition;
		IndexTableOffset = stream->readUInt32() + currentStreamPosition;
		NameTableOffset = stream->readUInt32() + currentStreamPosition;
	}
	catch (...) {
		return false;
	}

	return true;
}

bool J3DShapeBlock::Deserialize(bStream::CStream* stream) {
	size_t currentStreamPosition = stream->tell();

	if (!J3DBlock::Deserialize(stream))
		return false;

	try {
		Count = stream->readUInt16();
		stream->skip(2);

		InitDataTableOffset = stream->readUInt32() + currentStreamPosition;
		IndexTableOffset = stream->readUInt32() + currentStreamPosition;
		NameTableOffset = stream->readUInt32() + currentStreamPosition;
		VertexDescriptorTableOffset = stream->readUInt32() + currentStreamPosition;
		MatrixTableOffset = stream->readUInt32() + currentStreamPosition;
		DrawTableOffset = stream->readUInt32() + currentStreamPosition;
		MatrixInitTableOffset = stream->readUInt32() + currentStreamPosition;
		DrawInitDataTableOffset = stream->readUInt32() + currentStreamPosition;
	}
	catch (...) {
		return false;
	}

	return true;
}
