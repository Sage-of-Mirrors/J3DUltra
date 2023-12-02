#include "J3D/Skeleton/J3DJoint.hpp"
#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Texture/J3DTexture.hpp"

glm::mat4 J3DJoint::GetTransformMatrix() {
	std::shared_ptr<J3DJoint> p = std::dynamic_pointer_cast<J3DJoint>(shared_from_this());
	glm::mat4 completeTransform = glm::identity<glm::mat4>();

	while (p != nullptr) {
		completeTransform = p->mTransform.ToMat4() * completeTransform;
		p = std::dynamic_pointer_cast<J3DJoint>(p->mParent);
	}

	return completeTransform;
}

void J3DJoint::RenderRecursive(std::vector<std::shared_ptr<struct J3DTexture>>& textures) {
	//for (auto a : mMaterials) {
	//	//a->Render(textures);
	//}

	//for (auto c : mChildren) {
	//	static_cast<J3DJoint*>(c)->RenderRecursive(textures);
	//}
}
