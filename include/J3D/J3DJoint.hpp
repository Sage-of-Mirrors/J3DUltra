#pragma once

#include "J3DNode.hpp"
#include "J3DTransform.hpp"
#include "glm/glm.hpp"

enum class EJ3DJointBBoardType : uint8_t {
	None,
	XY,
	X
};

class J3DMatrixCalculator {
public:
	virtual void Init(glm::vec3 const& scale, glm::mat4 const& matrix) {}

	virtual void RecursiveUpdate(J3DNode* node) {}
	virtual void RecursiveCalculate(J3DNode* node) {}
	virtual void RecursiveEntry(J3DNode* node) {}

	virtual void CalculateTransform(uint16_t jointID, J3DTransformInfo const* transform) {}
	virtual void Calculate(uint16_t jointID) {}
};

class J3DMatrixCalculatorBasic : public virtual J3DMatrixCalculator {
	glm::mat4 mCurrentMatrix;
	glm::vec3 mCurrentScale;
	glm::vec3 mParentScale;

public:
	J3DMatrixCalculatorBasic();
	virtual ~J3DMatrixCalculatorBasic();

	virtual void Init(glm::vec3 const& scale, glm::mat4 const& matrix) {

	}

	virtual void RecursiveUpdate(J3DNode* node);
	virtual void RecursiveCalculate(J3DNode* node);
	virtual void RecursiveEntry(J3DNode* node);

	virtual void CalculateTransform(uint16_t jointID, J3DTransformInfo const* transform);
	virtual void Calculate(uint16_t jointID);
};

class J3DJoint : public J3DNode {
	uint16_t mJointID;
	uint8_t mMatrixFlag;
	uint8_t mAttachFlag;
	J3DTransformInfo mTransform;
	float mBoundingSphereRadius;
	glm::vec3 mBoundingBoxMin;
	glm::vec3 mBoundingBoxMax;

public:
	virtual void UpdateIn();
	virtual void UpdateOut();

	virtual void EntryIn();

	virtual void CalcIn();
	virtual void CalcOut();

	virtual EJ3DNodeType GetType() { return EJ3DNodeType::Joint; }
};
