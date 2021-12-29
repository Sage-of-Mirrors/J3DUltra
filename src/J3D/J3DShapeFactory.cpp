#include "J3D/J3DShapeFactory.hpp"
#include "J3D/J3DShape.hpp"
#include "J3D/J3DBlock.hpp"
#include "../bStream/bstream.h"

void J3DShapeInitData::Deserialize(bStream::CStream* stream) {
	MatrixType = stream->readUInt8();

	stream->skip(1);

	MatrixNum = stream->readUInt16();
	VCDOffset = stream->readUInt16();
	MatrixOffset = stream->readUInt16();
	DrawOffset = stream->readUInt16();

	stream->skip(2);

	BoundingSphereRadius = stream->readFloat();
	BoundingBoxMin = glm::vec3(stream->readFloat(), stream->readFloat(), stream->readFloat());
	BoundingBoxMax = glm::vec3(stream->readFloat(), stream->readFloat(), stream->readFloat());
}

J3DShape* J3DShapeFactory::Create(bStream::CStream* stream, uint32_t index) {
	J3DShape* newShape = new J3DShape();

	stream->seek(mBlock->InitDataTableOffset + (index * sizeof(J3DShapeInitData)));

	// Load the initial shape data
	J3DShapeInitData initData;
	initData.Deserialize(stream);

	// Load the vertex descriptions that will allow us to properly read the geometry data
	std::vector<J3DVCDData> vertexAttributes;
	stream->seek(mBlock->VertexDescriptorTableOffset + initData.VCDOffset);
	while ((EGXAttribute)stream->peekUInt32(stream->tell()) != EGXAttribute::Null) {
		J3DVCDData vcd = {
			(EGXAttribute)stream->readUInt32(),
			(EGXComponentType)stream->readUInt32()
		};

		vertexAttributes.push_back(vcd);
	}

	return newShape;
}
