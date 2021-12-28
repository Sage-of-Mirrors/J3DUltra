#include "J3D/J3DJoint.hpp"

void J3DMatrixCalculatorBasic::RecursiveUpdate(J3DNode* const node) {
	if (node == nullptr)
		return;

	node->UpdateIn();
	RecursiveUpdate(node->GetFirstChild());

	node->UpdateOut();
	RecursiveUpdate(node->GetNextNode());
}

void J3DMatrixCalculatorBasic::RecursiveCalculate(J3DNode* const node) {
	if (node == nullptr)
		return;

	node->CalcIn();
	RecursiveUpdate(node->GetFirstChild());

	node->CalcOut();
	RecursiveUpdate(node->GetNextNode());
}

void J3DMatrixCalculatorBasic::RecursiveEntry(J3DNode* const node) {
	if (node == nullptr)
		return;

	node->EntryIn();
	RecursiveEntry(node->GetFirstChild());
	RecursiveEntry(node->GetNextNode());
}
