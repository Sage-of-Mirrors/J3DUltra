#include "J3D/J3DTransform.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <bstream.h>

float J3DTransformInfo::U16ToFloat(int16_t val) {
	return val * (180 / 32768.0f);
}

uint16_t J3DTransformInfo::FloatToU16(float val) {
	return (uint16_t)(val * (32768.f / 180.f));
}

void J3DTransformInfo::Deserialize(bStream::CStream* stream) {
	Scale.x = stream->readFloat();
	Scale.y = stream->readFloat();
	Scale.z = stream->readFloat();

	glm::vec3 eulerRotation;
	eulerRotation.x = glm::radians(U16ToFloat(stream->readInt16()));
	eulerRotation.y = glm::radians(U16ToFloat(stream->readInt16()));
	eulerRotation.z = glm::radians(U16ToFloat(stream->readInt16()));

	Rotation = glm::angleAxis(eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
			   glm::angleAxis(eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
			   glm::angleAxis(eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));


	stream->skip(2);

	Translation.x = stream->readFloat();
	Translation.y = stream->readFloat();
	Translation.z = stream->readFloat();
}

glm::mat4 J3DTransformInfo::ToMat4() {
	return glm::translate(Translation) * glm::toMat4(Rotation) * glm::scale(Scale);
}
