#pragma once

#include <cstdint>
#include <vector>

enum class EJ3DNodeType {
	Base,
	Joint
};

class J3DNode {
protected:
	J3DNode* mParent;
	std::vector<J3DNode*> mChildren;

public:
	J3DNode() {}
	virtual ~J3DNode() {}

	virtual EJ3DNodeType GetType() const { return EJ3DNodeType::Base; }

	J3DNode* GetParent() { return mParent; }
	std::vector<J3DNode*> GetChildren() { return mChildren; }

	void AddChild(J3DNode* const newChild) {
		mChildren.push_back(newChild);
		newChild->mParent = this;
	}
};
