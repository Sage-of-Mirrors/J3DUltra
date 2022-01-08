#pragma once

class J3DShape;

#include <string>

class J3DMaterial {
	friend class J3DMaterialFactory;

	std::string mName;

	int32_t mShaderProgram;

	J3DShape* mShape;

public:
	J3DMaterial();
	~J3DMaterial();

	const J3DShape* GetShape() const { return mShape; }
	void SetShape(J3DShape* shape) { mShape = shape; }

	bool GenerateShaders(const int32_t& jointCount);

	void Bind();
	void Unbind();
};
