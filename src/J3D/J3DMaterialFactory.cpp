#include "J3D/J3DMaterialFactory.hpp"
#include "J3D/J3DBlock.hpp"
#include "J3D/J3DMaterial.hpp"
#include <bstream.h>

J3DMaterialFactory::J3DMaterialFactory(J3DMaterialBlock* srcBlock, bStream::CStream* stream) {
	mBlock = srcBlock;

	stream->seek(mBlock->IndexTableOffset);
	for (int i = 0; i < mBlock->Count; i++)
		mInstanceTable.push_back(stream->readUInt16());

	stream->seek(mBlock->NameTableOffset);
	mNameTable.Deserialize(stream);
}

J3DMaterial* J3DMaterialFactory::Create(bStream::CStream* stream, uint32_t index) {
	J3DMaterial* newMaterial = new J3DMaterial();
	newMaterial->mName = mNameTable.GetName(index);

	// Material init data can be "instanced", meaning that multiple materials use the same init data to load from file.
	// We can know which init data to use by looking up the proper index from the instance table.
	uint16_t instanceIndex = mInstanceTable[index];

	return newMaterial;
}
