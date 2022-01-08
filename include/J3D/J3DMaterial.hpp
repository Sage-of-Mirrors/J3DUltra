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

	bool GenerateShaders();

	void Bind();
	void Unbind();
};
