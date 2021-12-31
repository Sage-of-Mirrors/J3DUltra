#include "J3D/J3DTransform.hpp"
#include <bstream.h>

float J3DTransformInfo::U16ToFloat(uint16_t val) {
	return val * (180 / 32768.f);
}

uint16_t J3DTransformInfo::FloatToU16(float val) {
	return (uint16_t)(val * (32768.f / 180.f));
}

void J3DTransformInfo::Deserialize(bStream::CStream* stream) {
	Scale = glm::vec3(stream->readFloat(), stream->readFloat(), stream->readFloat());

	glm::vec3 eulerRotation = glm::vec3(U16ToFloat(stream->readUInt16()), U16ToFloat(stream->readUInt16()), U16ToFloat(stream->readUInt16()));
	Rotation = glm::angleAxis(eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
		       glm::angleAxis(eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
		       glm::angleAxis(eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));

	Translation = glm::vec3(stream->readFloat(), stream->readFloat(), stream->readFloat());
}
