#pragma once

#include <GXGeometryEnums.hpp>
#include <cstdint>

struct GXVertexAttributeList {
	EGXAttribute Attribute;
	EGXComponentCount ComponentCount;
	EGXComponentType ComponentType;
	uint8_t Fraction;
};