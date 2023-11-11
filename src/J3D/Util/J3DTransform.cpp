#include "J3D/Util/J3DTransform.hpp"

#include <bstream.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

float U16ToFloat(int16_t val) {
	return val * (180 / 32768.0f);
}

uint16_t FloatToU16(float val) {
	return (uint16_t)(val * (32768.f / 180.f));
}

/* == J3DTransformInfo == */
J3DTransformInfo::J3DTransformInfo()  : Scale(glm::vec3(1.0, 1.0, 1.0)), Rotation(glm::quat()), 
	Translation(glm::vec3(0.0, 0.0, 0.0)) {
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

bool J3DTransformInfo::operator==(const J3DTransformInfo& other) const {
	return Scale == other.Scale && Rotation == other.Rotation && Translation == other.Translation;
}

bool J3DTransformInfo::operator!=(const J3DTransformInfo& other) const {
	return !operator==(other);
}

/* == J3DTextureSRTInfo == */
void J3DTextureSRTInfo::Serialize(bStream::CStream* stream) {
	stream->writeFloat(Scale.x);
	stream->writeFloat(Scale.y);

	stream->writeUInt16(FloatToU16(glm::degrees(Rotation)));
	stream->writeUInt16(UINT16_MAX);

	stream->writeFloat(Translation.x);
	stream->writeFloat(Translation.y);
}

void J3DTextureSRTInfo::Deserialize(bStream::CStream* stream) {
	Scale.x = stream->readFloat();
	Scale.y = stream->readFloat();

	Rotation = glm::radians(U16ToFloat(stream->readInt16()));

	stream->skip(2);

	Translation.x = stream->readFloat();
	Translation.y = stream->readFloat();
}

glm::mat4 J3DTextureSRTInfo::ToMat4() {
	return glm::identity<glm::mat4>();
}

bool J3DTextureSRTInfo::operator==(const J3DTextureSRTInfo& other) const {
	return Scale == other.Scale && Rotation == other.Rotation && Translation == other.Translation;
}

bool J3DTextureSRTInfo::operator!=(const J3DTextureSRTInfo& other) const {
	return !operator==(other);
}
