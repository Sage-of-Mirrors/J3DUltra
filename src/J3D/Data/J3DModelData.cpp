#include "J3D/Data/J3DModelData.hpp"
#include "J3D/Data/J3DModelInstance.hpp"

#include "J3D/Skeleton/J3DSkeleton.hpp"
#include "J3D/Skeleton/J3DJoint.hpp"
#include "J3D/Skeleton/J3DNode.hpp"

#include "J3D/Material/J3DUniformBufferObject.hpp"

#include <glad/glad.h>

std::atomic<uint16_t> J3DModelData::sInstanceIdSrc = 1;

J3DModelData::J3DModelData() {
    mSkeleton = std::make_shared<J3DSkeleton>();
    mMaterialTable = std::make_shared<J3DMaterialTable>();
}

J3DModelData::~J3DModelData() {

}

void J3DModelData::MakeHierarchy(std::shared_ptr<J3DJoint> root, uint32_t& index) {
    std::shared_ptr<J3DJoint> last = root;
    const auto& shapes = mGeometry.GetShapes();

    while (true) {
        std::shared_ptr<J3DJoint> currentJoint = nullptr;
        std::shared_ptr<J3DMaterial> currentMaterial;
        const GXShape* currentShape = nullptr;

        switch (mHierarchyNodes[index].Type) {
            // The nodes after this one are lower on the hierarchy (eg go down to joint children)
        case EJ3DHierarchyType::Begin:
            index++;
            MakeHierarchy(last, index);

            break;
            // The nodes after this one are higher on the hierarchy (eg go back up to joint parents)
        case EJ3DHierarchyType::End:
            index++;

            return;
            // This is the last node in the hierarchy.
        case EJ3DHierarchyType::Exit:
            return;
            // This node represents a joint, so grab that joint.
        case EJ3DHierarchyType::Joint:
            currentJoint = mSkeleton->GetJoint(mHierarchyNodes[index].Index);
            index++;

            break;
            // This node represents a material, so grab that material.
        case EJ3DHierarchyType::Material:
            currentMaterial = mMaterialTable->GetMaterial(mHierarchyNodes[index].Index);
            index++;

            break;
            // This node represents a shape, so grab that shape.
        case EJ3DHierarchyType::Shape:
            currentShape = shapes[mHierarchyNodes[index].Index];
            index++;

            break;
        }

        // If we have a joint this iteration, set it as the last joint we've seen and add it to the hierarchy.
        if (currentJoint != nullptr) {
            last = currentJoint;

            if (root == nullptr) {
                mSkeleton->SetRootJoint(currentJoint);
            }
            else {
                root->AddChild(currentJoint);
            }
        }
        // If we have a material this iteration, add it to the current root joint.
        else if (currentMaterial != nullptr)
            root->AddMaterial(currentMaterial);
        // If we have a shape this iteration, assign it to the last material we added to the current root joint.
        // Also generate shaders, since now that it has a shape the material has all the data it needs.
        else if (currentShape != nullptr) {
            std::shared_ptr<J3DMaterial> shapeMaterial = root->GetLastMaterial();

            shapeMaterial->SetShape(currentShape);
            shapeMaterial->GenerateShaders();
            J3DUniformBufferObject::LinkMaterialToUBO(shapeMaterial);
        }
    }
}

void J3DModelData::CalculateRestPose() {
    mSkeleton->CalculateRestPose();
}

bool J3DModelData::InitializeGL() {
    mGeometry.CreateVertexArray();

    const auto& verts = mGeometry.GetModelVertices();
    const auto& indices = mGeometry.GetModelIndices();

    // Create VBO
    glCreateBuffers(1, &mVBO);
    glNamedBufferStorage(mVBO, verts.size() * sizeof(ModernVertex), verts.data(), GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);

    // Create IBO
    glCreateBuffers(1, &mIBO);
    glNamedBufferStorage(mIBO, indices.size() * sizeof(uint32_t), indices.data(), GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);

    // Create VAO
    glCreateVertexArrays(1, &mVAO);
    if (mVAO == UINT32_MAX)
        return false;

    // Set VBO as the data source for the VAO
    glVertexArrayVertexBuffer(mVAO, 0, mVBO, 0, sizeof(ModernVertex));
    // Set IBO as the index source for the VAO
    glVertexArrayElementBuffer(mVAO, mIBO);

    // Configure position data on VAO
    if (mVertexData.HasPositions()) {
        uint32_t posEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::Position);
        glEnableVertexArrayAttrib(mVAO, posEnumVal);

        glVertexArrayAttribBinding(mVAO, posEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, posEnumVal, glm::vec4::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, Position));
    }

    // Configure normal data on VAO
    if (mVertexData.HasNormals()) {
        uint32_t nrmEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::Normal);
        glEnableVertexArrayAttrib(mVAO, nrmEnumVal);

        glVertexArrayAttribBinding(mVAO, nrmEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, nrmEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, Normal));
    }

    // Configure vertex color data on VAO
    if (mVertexData.HasColors(0)) {
        uint32_t colEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::Color0);
        glEnableVertexArrayAttrib(mVAO, colEnumVal);

        glVertexArrayAttribBinding(mVAO, colEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, colEnumVal, glm::vec4::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, Colors[0]));
    }
    if (mVertexData.HasColors(1)) {
        uint32_t colEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::Color1);
        glEnableVertexArrayAttrib(mVAO, colEnumVal);

        glVertexArrayAttribBinding(mVAO, colEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, colEnumVal, glm::vec4::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, Colors[1]));
    }

    // Configure tex coord data on VAO
    if (mVertexData.HasTexCoords(0)) {
        uint32_t texEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::TexCoord0);
        glEnableVertexArrayAttrib(mVAO, texEnumVal);

        glVertexArrayAttribBinding(mVAO, texEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, texEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, TexCoords[0]));
    }
    if (mVertexData.HasTexCoords(1)) {
        uint32_t texEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::TexCoord1);
        glEnableVertexArrayAttrib(mVAO, texEnumVal);

        glVertexArrayAttribBinding(mVAO, texEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, texEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, TexCoords[1]));
    }
    if (mVertexData.HasTexCoords(2)) {
        uint32_t texEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::TexCoord2);
        glEnableVertexArrayAttrib(mVAO, texEnumVal);

        glVertexArrayAttribBinding(mVAO, texEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, texEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, TexCoords[2]));
    }
    if (mVertexData.HasTexCoords(3)) {
        uint32_t texEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::TexCoord3);
        glEnableVertexArrayAttrib(mVAO, texEnumVal);

        glVertexArrayAttribBinding(mVAO, texEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, texEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, TexCoords[3]));
    }
    if (mVertexData.HasTexCoords(4)) {
        uint32_t texEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::TexCoord4);
        glEnableVertexArrayAttrib(mVAO, texEnumVal);

        glVertexArrayAttribBinding(mVAO, texEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, texEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, TexCoords[4]));
    }
    if (mVertexData.HasTexCoords(5)) {
        uint32_t texEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::TexCoord5);
        glEnableVertexArrayAttrib(mVAO, texEnumVal);

        glVertexArrayAttribBinding(mVAO, texEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, texEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, TexCoords[5]));
    }
    if (mVertexData.HasTexCoords(6)) {
        uint32_t texEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::TexCoord6);
        glEnableVertexArrayAttrib(mVAO, texEnumVal);

        glVertexArrayAttribBinding(mVAO, texEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, texEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, TexCoords[6]));
    }
    if (mVertexData.HasTexCoords(7)) {
        uint32_t texEnumVal = J3DUtility::EnumToIntegral(EGXAttribute::TexCoord7);
        glEnableVertexArrayAttrib(mVAO, texEnumVal);

        glVertexArrayAttribBinding(mVAO, texEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, texEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(ModernVertex, TexCoords[7]));
    }

    return true;
}

std::shared_ptr<J3DModelInstance> J3DModelData::CreateInstance() {
    return std::make_shared<J3DModelInstance>(shared_from_this(), sInstanceIdSrc++);
}

std::vector<glm::mat4> J3DModelData::GetRestPose() const {
    return mSkeleton->GetRestPose();
}

bool J3DModelData::SetTexture(uint32_t idx, uint32_t width, uint32_t height, uint8_t* data, uint32_t size) {
    return mMaterialTable->SetTexture(idx, width, height, data, size);
}

bool J3DModelData::SetTexture(std::string name, uint32_t width, uint32_t height, uint8_t* data, uint32_t size) {
    return mMaterialTable->SetTexture(name, width, height, data, size);
}

void J3DModelData::BindVAO()
{
    if (!mGLInitialized)
        mGLInitialized = InitializeGL();

    glBindVertexArray(mVAO);
}

void J3DModelData::UnbindVAO()
{
    glBindVertexArray(0);
}
