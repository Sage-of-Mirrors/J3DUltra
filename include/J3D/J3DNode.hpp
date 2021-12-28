#pragma once

#include <cstdint>
#include <vector>

enum class EJ3DNodeType {
	Base,
	Joint
};

class J3DNode {
	J3DNode* mParent;
	std::vector<J3DNode*> mChildren;

public:
	J3DNode();
	virtual ~J3DNode();

	virtual EJ3DNodeType GetType() const { return EJ3DNodeType::Base; }

	J3DNode* GetParent() { return mParent; }
	std::vector<J3DNode*> GetChildren() { return mChildren; }
};
