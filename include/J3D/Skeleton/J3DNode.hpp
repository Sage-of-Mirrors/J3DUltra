#pragma once

#include "J3D/Util/J3DUtil.hpp"

#include <cstdint>
#include <vector>
#include <memory>


enum class EJ3DNodeType {
	Base,
	Joint
};

class J3DNode : public std::enable_shared_from_this<J3DNode> {
protected:
	std::weak_ptr<J3DNode> mParent;
	std::vector<std::weak_ptr<J3DNode>> mChildren;

public:
	J3DNode() : mParent(std::shared_ptr<J3DNode>()) {}
	virtual ~J3DNode() {}

	virtual EJ3DNodeType GetType() const { return EJ3DNodeType::Base; }

	std::weak_ptr<J3DNode> GetParent() { return mParent; }
	std::vector<std::weak_ptr<J3DNode>>& GetChildren() { return mChildren; }

	void AddChild(std::shared_ptr<J3DNode> newChild) {
		mChildren.push_back(newChild);
		newChild->mParent = shared_from_this();
	}
};
