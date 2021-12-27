#pragma once

#include <cstdint>
#include <vector>

class J3DModelLoader;

class J3DModelData {
	friend J3DModelLoader;

	uint32_t mFlags;
	uint32_t mMatrixGroupCount;

	std::vector<J3DModelHierarchy> mHierarchyNodes;

	void MakeHierarchy(void* const root, std::vector<J3DModelHierarchy>* nodes);

public:
	J3DModelData();
	virtual ~J3DModelData();


};
