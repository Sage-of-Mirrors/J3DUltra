#include "J3D/J3DShapeFactory.hpp"
#include "J3D/J3DShape.hpp"
#include "J3D/J3DBlock.hpp"
#include "GX/GXEnum.hpp"
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

	// Now load the packet data. This'll be a doozy
	for (int i = 0; i < initData.MatrixNum; i++) {
		J3DPacket newPacket;

		uint32_t drawInitOffset = (initData.DrawOffset + i) * sizeof(J3DShapeDrawInitData);
		stream->seek(mBlock->DrawInitDataTableOffset + drawInitOffset);

		// Load info about the packet - size and offset of the primitives making it up.
		J3DShapeDrawInitData drawInitData{
			stream->readUInt16(),
			stream->readUInt16()
		};

		stream->seek(mBlock->DrawTableOffset + drawInitData.Start);

		uint32_t endOffset = stream->tell() + drawInitData.Size;
		while (stream->tell() < endOffset) {
			std::vector<J3DVertex> primitiveVertices;

			// Get the primitive type. If it's None (0), we're done reading the packet's primitives
			// because the rest of the bytes are padding.
			EGXPrimitiveType primType = (EGXPrimitiveType)stream->readUInt8();
			if (primType == EGXPrimitiveType::None)
				break;

			uint16_t vtxCount = stream->readUInt16();
			for (int i = 0; i < vtxCount; i++) {
				J3DVertex newVtx;

				for (auto attribute : vertexAttributes) {
					int16_t value = 0;

					// Read the index value
					switch (attribute.Type) {
						case EGXComponentType::Unsigned16:
						case EGXComponentType::Signed16:
							value = stream->readInt16();
							break;
						case EGXComponentType::Unsigned8:
						case EGXComponentType::Signed8:
							value = stream->readInt8();
							break;
					}

					// Assign it to the proper member of the vertex
					switch (attribute.Attribute) {
						case EGXAttribute::PositionMatrixIdx:
							// Special case! Need to process this into 2 values.
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

				primitiveVertices.push_back(newVtx);
			}

			std::vector<J3DVertex> triangulatedVertices = TriangulatePrimitive(primType, primitiveVertices);
			newPacket.mVertices.insert(newPacket.mVertices.end(), triangulatedVertices.begin(), triangulatedVertices.end());
		}

		newShape->mPackets.push_back(newPacket);
	}

	return newShape;
}

std::vector<J3DVertex> J3DShapeFactory::TriangulatePrimitive(EGXPrimitiveType primType, std::vector<J3DVertex> const& vertices) {
	switch (primType) {
		case EGXPrimitiveType::Triangles:
			return vertices;
		case EGXPrimitiveType::TriangleStrips:
			return TriangulateTriangleStrip(vertices);
		case EGXPrimitiveType::TriangleFan:
			return TriangulateTriangleFan(vertices);
		default:
			return std::vector<J3DVertex>();
	}
}

std::vector<J3DVertex> J3DShapeFactory::TriangulateTriangleStrip(std::vector<J3DVertex> const& vertices) {
	std::vector<J3DVertex> triangles;

	for (int i = 2; i < vertices.size(); i++) {
		bool isIndexOdd = i % 2 != 0;

		J3DVertex const* newTriangle[3];

		newTriangle[0] = &vertices[i - 2];
		newTriangle[1] = isIndexOdd ? &vertices[i] : &vertices[i - 1];
		newTriangle[2] = isIndexOdd ? &vertices[i - 1] : &vertices[i];

		// Reject degenerate triangles (triangles where two or more vertices are the same)
		if (newTriangle[0] != newTriangle[1] && newTriangle[1] != newTriangle[2] && newTriangle[2] != newTriangle[0]) {
			triangles.push_back(*newTriangle[0]);
			triangles.push_back(*newTriangle[1]);
			triangles.push_back(*newTriangle[2]);
		}
	}

	return triangles;
}

std::vector<J3DVertex> J3DShapeFactory::TriangulateTriangleFan(std::vector<J3DVertex> const& vertices) {
	std::vector<J3DVertex> triangles;

	for (int i = 1; i < vertices.size() - 1; i++) {
		J3DVertex const* newTriangle[3];

		newTriangle[0] = &vertices[i];
		newTriangle[1] = &vertices[i + 1];
		newTriangle[2] = &vertices[0];

		// Reject degenerate triangles (triangles where two or more vertices are the same)
		if (newTriangle[0] != newTriangle[1] && newTriangle[1] != newTriangle[2] && newTriangle[2] != newTriangle[0]) {
			triangles.push_back(*newTriangle[0]);
			triangles.push_back(*newTriangle[1]);
			triangles.push_back(*newTriangle[2]);
		}
	}

	return triangles;
}
