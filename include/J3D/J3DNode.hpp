#pragma once

#include <cstdint>
#include <vector>

class J3DModelData;

enum class EJ3DNodeType {
	Base,
	Joint
};

class J3DNode {
	J3DNode* mNextNode;
	J3DNode* mChildNode;

public:
	J3DNode();
	virtual ~J3DNode();

	virtual void Init(J3DModelData* modelData) {}

	virtual void UpdateIn() {}
	virtual void UpdateOut() {}
	
	virtual void EntryIn() {}

	virtual void CalcIn() {}
	virtual void CalcOut() {}

	virtual EJ3DNodeType GetType() const { return EJ3DNodeType::Base; }

	J3DNode* GetNextNode() { return mNextNode; }
	J3DNode* GetFirstChild() { return mChildNode; }

	void AddChild(J3DNode* child);
};
