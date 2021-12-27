#include "J3D/J3DModelLoader.hpp"
#include "J3D/J3DModelData.hpp"
#include "../lib/bStream/bstream.h"

J3DModelLoader::J3DModelLoader() : mModelData(nullptr) {

}

J3DModelData* J3DModelLoader::Load(bStream::CStream* stream, uint32_t flags) {
	mModelData = new J3DModelData();
	
	J3DDataBase header;
	header.Deserialize(stream);

	for (int i = 0; i < header.BlockCount; i++) {
		switch ((EJ3DBlockType)stream->peekUInt32(0)) {
			case EJ3DBlockType::INF1:
				ReadInformationBlock(stream, flags);
				break;
		}
	}

	mModelData->MakeHierarchy(nullptr, &mModelData->mHierarchyNodes);

	return mModelData;
}

void J3DModelLoader::ReadInformationBlock(bStream::CStream* stream, uint32_t flags) {
	size_t currentStreamPos = stream->tell();

	J3DModelInfoBlock infoBlock;
	infoBlock.Deserialize(stream);

	mModelData->mFlags = flags | infoBlock.Flags;

	// TODO: Create the matrix calc types and assign them here
	// Set up the matrix calculator this model should use - basic, Maya, or SoftImage.
	switch (mModelData->mFlags & FLAGS_MATRIX_MASK) {
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
	}

	mModelData->mMatrixGroupCount = infoBlock.MatrixGroupCount;
	//mModelData->mVertexData.PositionCount = infoBlock->VertexPositionCount;

	stream->seek(infoBlock.HierarchyOffset);

	J3DModelHierarchy hierarchyNode;
	hierarchyNode.Type = (EJ3DHierarchyType)stream->readUInt16();
	hierarchyNode.Index = stream->readUInt16();

	mModelData->mHierarchyNodes.push_back(hierarchyNode);

	while (hierarchyNode.Type != EJ3DHierarchyType::Exit) {
		hierarchyNode.Type = (EJ3DHierarchyType)stream->readUInt16();
		hierarchyNode.Index = stream->readUInt16();

		mModelData->mHierarchyNodes.push_back(hierarchyNode);
	}

	stream->seek(currentStreamPos + infoBlock.BlockSize);
}
