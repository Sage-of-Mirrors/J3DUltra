#pragma once

#include <cstdint>
#include "GXEnum.hpp"

struct GXVertexAttributeList {
	EGXAttribute Attribute;
	EGXComponentCount ComponentCount;
	EGXComponentType ComponentType;
	uint8_t Fraction;
};