#include "J3D/J3DShapeFactory.hpp"
#include "J3D/J3DShape.hpp"
#include "J3D/J3DBlock.hpp"
#include "GX/GXEnum.hpp"
#include <bstream.h>

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
			(EGXAttributeType)stream->readUInt32()
		};

		vertexAttributes.push_back(vcd);
	}

	newShape->EnableAttributes(vertexAttributes);

	// Now load the packet data. This'll be a doozy
	for (int packetIndex = 0; packetIndex < initData.MatrixNum; packetIndex++) {
		J3DPacket newPacket;

		uint32_t drawInitOffset = (initData.DrawOffset + packetIndex) * sizeof(J3DShapeDrawInitData);
		stream->seek(mBlock->DrawInitDataTableOffset + drawInitOffset);

		// Load info about the packet - size and offset of the primitives making it up.
		J3DShapeDrawInitData drawInitData{
			stream->readUInt32(),
			stream->readUInt32()
		};

		stream->seek(mBlock->DrawTableOffset + drawInitData.Start);

		uint32_t endOffset = stream->tell() + drawInitData.Size;
		while (stream->tell() < endOffset) {
			std::vector<J3DVertexGX> primitiveVertices;
			std::vector<uint16_t> primitiveDrawIndices;

			// Get the primitive type. If it's None (0), we're done reading the packet's primitives
			// because the rest of the bytes are padding.
			EGXPrimitiveType primType = (EGXPrimitiveType)stream->readUInt8();
			if (primType == EGXPrimitiveType::None)
				break;

			uint16_t vtxCount = stream->readUInt16();
			for (int j = 0; j < vtxCount; j++) {
				J3DVertexGX newVtx;

				for (auto attribute : vertexAttributes) {
					uint16_t value = 0;

					// Read the index value
					switch (attribute.Type) {
						case EGXAttributeType::Index16:
							value = stream->readUInt16();
							break;
						case EGXAttributeType::Index8:
							value = stream->readUInt8();
							break;
						case EGXAttributeType::Direct:
							if (attribute.Attribute != EGXAttribute::PositionMatrixIdx) {
								std::cout << "Found a Direct attribute that wasn't PnMtxIdx!!! (Value: " << (uint32_t)attribute.Attribute << ")\n";
								continue;
							}

							value = stream->readUInt8();
							break;
						case EGXAttributeType::None:
						default:
							continue;
					}

					// Assign it to the proper member of the vertex
					switch (attribute.Attribute) {
						case EGXAttribute::PositionMatrixIdx:
							// Special case! We will calculate the index into DRW1 to use later, and store it in the draw indices vector.
							
							// Divide by 3 because ???
							value /= 3;
							newVtx.DrawIndex = ConvertPosMtxIndexToDrawIndex(stream, initData, packetIndex, value);
							break;
						case EGXAttribute::Position:
							newVtx.Position = value;
							break;
						case EGXAttribute::Normal:
							newVtx.Normal = value;
							break;
						case EGXAttribute::Color0:
						case EGXAttribute::Color1:
							newVtx.Color[(uint32_t)attribute.Attribute - (uint32_t)EGXAttribute::Color0] = value;
							break;
						case EGXAttribute::TexCoord0:
						case EGXAttribute::TexCoord1:
						case EGXAttribute::TexCoord2:
						case EGXAttribute::TexCoord3:
						case EGXAttribute::TexCoord4:
						case EGXAttribute::TexCoord5:
						case EGXAttribute::TexCoord6:
						case EGXAttribute::TexCoord7:
							newVtx.TexCoord[(uint32_t)attribute.Attribute - (uint32_t)EGXAttribute::TexCoord0] = value;
							break;
					}
				}

				if (initData.MatrixType == 0)
					newVtx.DrawIndex = GetUseMatrixValue(stream, initData, packetIndex);

				primitiveVertices.push_back(newVtx);
			}

			std::vector<J3DVertexGX> triangulatedVertices = TriangulatePrimitive<J3DVertexGX>(primType, primitiveVertices);
			newPacket.mVertices.insert(newPacket.mVertices.end(), triangulatedVertices.begin(), triangulatedVertices.end());
		}

		newShape->mPackets.push_back(newPacket);
	}

	return newShape;
}

uint16_t J3DShapeFactory::ConvertPosMtxIndexToDrawIndex(bStream::CStream* stream, const J3DShapeInitData& initData, const uint16_t& packetIndex, const uint16_t& value) {
	uint16_t index = 0;
	
	uint32_t currentStreamPos = stream->tell();
	stream->seek(mBlock->MatrixInitTableOffset + (initData.MatrixOffset * sizeof(J3DShapeMatrixInitData)));

	uint16_t matrixInitIndex = initData.MatrixOffset + packetIndex;
	J3DShapeMatrixInitData matrixInitData;

	while (matrixInitIndex >= 0) {
		// Grab the matrix data
		ReadMatrixInitData(stream, matrixInitData, matrixInitIndex * sizeof(J3DShapeMatrixInitData));

		// calculate the offset to read from
		uint32_t matrixTableOffset = mBlock->MatrixTableOffset + (matrixInitData.Start + value) * sizeof(uint16_t);

		// Read the value !
		uint16_t matrixEntry = stream->peekUInt16(matrixTableOffset);
		// If the index we read isn't 0xFFFF, we can finish early because we have our value.
		if (matrixEntry != 0xFFFF) {
			index = matrixEntry;
			break;
		}

		// The value we read was 0xFFFF, which means we need to move up a matrix entry to try and grab
		// another value in the same position. This can happen multiple times. :(
		matrixInitIndex--;
	}

	stream->seek(currentStreamPos);
	return index;
}

uint16_t J3DShapeFactory::GetUseMatrixValue(bStream::CStream* stream, const J3DShapeInitData& initData, const uint16_t& packetIndex) {
	uint16_t index = 0;

	uint32_t currentStreamPos = stream->tell();
	stream->seek(mBlock->MatrixInitTableOffset + (initData.MatrixOffset * sizeof(J3DShapeMatrixInitData)));

	uint32_t matrixInitIndex = initData.MatrixOffset + packetIndex;
	
	J3DShapeMatrixInitData matrixInitData;
	ReadMatrixInitData(stream, matrixInitData, matrixInitIndex * sizeof(J3DShapeMatrixInitData));

	index = matrixInitData.ID;

	stream->seek(currentStreamPos);
	return index;
}

void J3DShapeFactory::ReadMatrixInitData(bStream::CStream* stream, J3DShapeMatrixInitData& data, uint32_t offset) {
	uint32_t currentStreamPos = stream->tell();
	stream->seek(mBlock->MatrixInitTableOffset + offset);

	data.ID = stream->readUInt16();
	data.Count = stream->readUInt16();
	data.Start = stream->readUInt32();

	stream->seek(currentStreamPos);
}
