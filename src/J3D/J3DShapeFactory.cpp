#include "J3D/J3DShapeFactory.hpp"
#include "J3D/J3DShape.hpp"
#include "J3D/J3DBlock.hpp"

#include <GXGeometryData.hpp>
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

GXShape* J3DShapeFactory::Create(bStream::CStream* stream, uint32_t index) {
	GXShape* gxShape = new GXShape();
	//J3DShape* j3dShape = new J3DShape();

	stream->seek(mBlock->InitDataTableOffset + (index * sizeof(J3DShapeInitData)));

	// Load the initial shape data
	J3DShapeInitData initData;
	initData.Deserialize(stream);

	// Load the vertex descriptions that will allow us to properly read the geometry data
	auto& shapeAttributeTable = gxShape->GetAttributeTable();
	std::vector<J3DVCDData> vertexAttributes;

	stream->seek(mBlock->VertexDescriptorTableOffset + initData.VCDOffset);
	while ((EGXAttribute)stream->peekUInt32(stream->tell()) != EGXAttribute::Null) {
		J3DVCDData vcd = {
			(EGXAttribute)stream->readUInt32(),
			(EGXAttributeIndexType)stream->readUInt32()
		};

		shapeAttributeTable.push_back(vcd.Attribute);
		vertexAttributes.push_back(vcd);
	}

	auto& shapePrimitives = gxShape->GetPrimitives();

	// Load the primitive data per-packet so we can properly extract the skinning info
	for (int packetIndex = 0; packetIndex < initData.MatrixNum; packetIndex++) {
		uint32_t drawInitOffset = (initData.DrawOffset + packetIndex) * sizeof(J3DShapeDrawInitData);
		stream->seek(mBlock->DrawInitDataTableOffset + drawInitOffset);

		// Load info about the packet - size and offset of the primitives making it up.
		J3DShapeDrawInitData drawInitData{
			stream->readUInt32(),
			stream->readUInt32()
		};

		stream->seek(mBlock->DrawTableOffset + drawInitData.Start);
		uint32_t endOffset = stream->tell() + drawInitData.Size;

		// Load primitives until we're out of space
		while (stream->tell() < endOffset) {
			// Get the primitive type. If it's None (0), we're done reading the packet's primitives
			// because the rest of the bytes are padding.
			EGXPrimitiveType primType = (EGXPrimitiveType)stream->readUInt8();
			if (primType == EGXPrimitiveType::None)
				break;

			GXPrimitive* newPrimitive = new GXPrimitive(primType);
			auto& primitiveVerts = newPrimitive->GetVertices();

			uint16_t vtxCount = stream->readUInt16();
			for (int j = 0; j < vtxCount; j++) {
				GXVertex newVtx;

				for (auto attribute : vertexAttributes) {
					uint16_t value = 0;

					// Read the index value
					switch (attribute.Type) {
						case EGXAttributeIndexType::Index16:
							value = stream->readUInt16();
							break;
						case EGXAttributeIndexType::Index8:
							value = stream->readUInt8();
							break;
						case EGXAttributeIndexType::Direct:
							if (attribute.Attribute != EGXAttribute::PositionMatrixIdx) {
								std::cout << "Found a Direct attribute that wasn't PnMtxIdx!!! (Value: " << (uint32_t)attribute.Attribute << ")\n";
								continue;
							}

							value = stream->readUInt8();
							break;
						case EGXAttributeIndexType::None:
						default:
							continue;
					}

					// Assign it to the proper member of the vertex
					switch (attribute.Attribute) {
						case EGXAttribute::PositionMatrixIdx:
							// Special case! We will calculate the index into DRW1 to use later, and store it in the draw indices vector.
							
							// Divide by 3 because ???
							value /= 3;
							newVtx.SetIndex(attribute.Attribute, ConvertPosMtxIndexToDrawIndex(stream, initData, packetIndex, value));
							break;
						case EGXAttribute::Position:
						case EGXAttribute::Normal:
						case EGXAttribute::Color0:
						case EGXAttribute::Color1:
						case EGXAttribute::TexCoord0:
						case EGXAttribute::TexCoord1:
						case EGXAttribute::TexCoord2:
						case EGXAttribute::TexCoord3:
						case EGXAttribute::TexCoord4:
						case EGXAttribute::TexCoord5:
						case EGXAttribute::TexCoord6:
						case EGXAttribute::TexCoord7:
							newVtx.SetIndex(attribute.Attribute, value);
							break;
					}
				}

				if (initData.MatrixType == 0)
					newVtx.SetIndex(EGXAttribute::PositionMatrixIdx, GetUseMatrixValue(stream, initData, packetIndex));

				primitiveVerts.push_back(newVtx);
			}

			shapePrimitives.push_back(newPrimitive);
		}
	}

	return gxShape;
}

uint16_t J3DShapeFactory::ConvertPosMtxIndexToDrawIndex(bStream::CStream* stream, const J3DShapeInitData& initData, const uint16_t& packetIndex, const uint16_t& value) {
	uint32_t index = 0;
	
	uint32_t currentStreamPos = stream->tell();
	stream->seek(mBlock->MatrixInitTableOffset + (initData.MatrixOffset * sizeof(J3DShapeMatrixInitData)));

	uint32_t matrixInitIndex = initData.MatrixOffset + packetIndex;
	J3DShapeMatrixInitData matrixInitData;

	while (matrixInitIndex >= 0) {
		// Grab the matrix data
		ReadMatrixInitData(stream, matrixInitData, matrixInitIndex * sizeof(J3DShapeMatrixInitData));

		// calculate the offset to read from
		uint32_t matrixTableOffset = mBlock->MatrixTableOffset + (matrixInitData.Start + value) * sizeof(uint16_t);

		// Read the value !
		uint32_t matrixEntry = stream->peekUInt16(matrixTableOffset);
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
	uint32_t index = 0;

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
