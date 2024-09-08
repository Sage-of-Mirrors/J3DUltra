#include "J3D/Geometry/J3DShapeFactory.hpp"
#include "J3D/Geometry/J3DShape.hpp"
#include "J3D/Data/J3DBlock.hpp"

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

std::shared_ptr<GXShape> J3DShapeFactory::Create(bStream::CStream* stream, uint32_t index, const GXAttributeData* attributes) {
	std::shared_ptr<GXShape> gxShape = std::make_shared<GXShape>();

	stream->seek(mBlock->InitDataTableOffset + (index * sizeof(J3DShapeInitData)));

	// Load the initial shape data
	J3DShapeInitData initData;
	initData.Deserialize(stream);

	gxShape->SetUserData(new uint32_t(initData.MatrixType));

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

	shapeAttributeTable.shrink_to_fit();
	vertexAttributes.shrink_to_fit();

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
				ModernVertex newVtx;

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
							value = stream->readUInt8();
							break;
						case EGXAttributeIndexType::None:
						default:
							continue;
					}

					// Assign it to the proper member of the vertex
					switch (attribute.Attribute) {
						/* Matrix indices */
						case EGXAttribute::PositionMatrixIdx:
							newVtx.Position.w = static_cast<float>(ConvertPosMtxIndexToDrawIndex(stream, initData, packetIndex, value / 3));
							break;
						case EGXAttribute::Tex0MatrixIdx:
							newVtx.TexCoords[0].z = value / 3.0f;
							break;
						case EGXAttribute::Tex1MatrixIdx:
							newVtx.TexCoords[1].z = value / 3.0f;
							break;
						case EGXAttribute::Tex2MatrixIdx:
							newVtx.TexCoords[2].z = value / 3.0f;
							break;
						case EGXAttribute::Tex3MatrixIdx:
							newVtx.TexCoords[3].z = value / 3.0f;
							break;
						case EGXAttribute::Tex4MatrixIdx:
							newVtx.TexCoords[4].z = value / 3.0f;
							break;
						case EGXAttribute::Tex5MatrixIdx:
							newVtx.TexCoords[5].z = value / 3.0f;
							break;
						case EGXAttribute::Tex6MatrixIdx:
							newVtx.TexCoords[6].z = value / 3.0f;
							break;
						case EGXAttribute::Tex7MatrixIdx:
							newVtx.TexCoords[7].z = value / 3.0f;
							break;

						/* Typical vertex attributes */
						case EGXAttribute::Position:
						{
							glm::vec4 pos = attributes->GetPositions()[value];
							newVtx.Position.x = pos.x;
							newVtx.Position.y = pos.y;
							newVtx.Position.z = pos.z;
							break;
						}
						case EGXAttribute::Normal:
							newVtx.Normal = attributes->GetNormals()[value];
							break;
						case EGXAttribute::Color0:
							newVtx.Colors[0] = attributes->GetColors(0)[value];
							break;
						case EGXAttribute::Color1:
							newVtx.Colors[1] = attributes->GetColors(1)[value];
							break;
						case EGXAttribute::TexCoord0:
						{
							glm::vec3 texCoord = attributes->GetTexCoords(0)[value];
							newVtx.TexCoords[0].x = texCoord.x;
							newVtx.TexCoords[0].y = texCoord.y;
							break;
						}
						case EGXAttribute::TexCoord1:
						{
							glm::vec3 texCoord = attributes->GetTexCoords(1)[value];
							newVtx.TexCoords[1].x = texCoord.x;
							newVtx.TexCoords[1].y = texCoord.y;
							break;
						}
						case EGXAttribute::TexCoord2:
						{
							glm::vec3 texCoord = attributes->GetTexCoords(2)[value];
							newVtx.TexCoords[2].x = texCoord.x;
							newVtx.TexCoords[2].y = texCoord.y;
							break;
						}
						case EGXAttribute::TexCoord3:
						{
							glm::vec3 texCoord = attributes->GetTexCoords(3)[value];
							newVtx.TexCoords[3].x = texCoord.x;
							newVtx.TexCoords[3].y = texCoord.y;
							break;
						}
						case EGXAttribute::TexCoord4:
						{
							glm::vec3 texCoord = attributes->GetTexCoords(4)[value];
							newVtx.TexCoords[4].x = texCoord.x;
							newVtx.TexCoords[4].y = texCoord.y;
							break;
						}
						case EGXAttribute::TexCoord5:
						{
							glm::vec3 texCoord = attributes->GetTexCoords(5)[value];
							newVtx.TexCoords[5].x = texCoord.x;
							newVtx.TexCoords[5].y = texCoord.y;
							break;
						}
						case EGXAttribute::TexCoord6:
						{
							glm::vec3 texCoord = attributes->GetTexCoords(6)[value];
							newVtx.TexCoords[6].x = texCoord.x;
							newVtx.TexCoords[6].y = texCoord.y;
							break;
						}
						case EGXAttribute::TexCoord7:
						{
							glm::vec3 texCoord = attributes->GetTexCoords(7)[value];
							newVtx.TexCoords[7].x = texCoord.x;
							newVtx.TexCoords[7].y = texCoord.y;
							break;
						}
					}
				}

				if (initData.MatrixType == 0) {
					newVtx.Position.w = GetUseMatrixValue(stream, initData, packetIndex);
				}

				primitiveVerts.push_back(newVtx);
			}

			primitiveVerts.shrink_to_fit();

			//newPrimitive->TriangluatePrimitive();
			shapePrimitives.push_back(newPrimitive);
		}
	}

	shapePrimitives.shrink_to_fit();

	gxShape->CalculateCenterOfMass();
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
