#include "J3D/J3DModelData.hpp"
#include "J3D/J3DNode.hpp"
#include "J3D/J3DUtil.hpp"
#include "J3D/J3DUniformBufferObject.hpp"
#include <glad/glad.h>

void J3DModelData::MakeHierarchy(J3DJoint* const root, uint32_t& index) {
    J3DJoint* last = root;

    while (true) {
        J3DJoint* currentJoint = nullptr;
        J3DMaterial* currentMaterial = nullptr;
        J3DShape* currentShape = nullptr;

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
            currentJoint = mJoints[mHierarchyNodes[index].Index];
            index++;

            break;
            // This node represents a material, so grab that material.
        case EJ3DHierarchyType::Material:
            currentMaterial = mMaterials[mHierarchyNodes[index].Index];
            index++;

            break;
            // This node represents a shape, so grab that shape.
        case EJ3DHierarchyType::Shape:
            currentShape = mShapes[mHierarchyNodes[index].Index];
            index++;

            break;
        }

        // If we have a joint this iteration, set it as the last joint we've seen and add it to the hierarchy.
        if (currentJoint != nullptr) {
            last = currentJoint;

            if (root == nullptr)
                mRootJoint = currentJoint;
            else
                root->AddChild(currentJoint);
        }
        // If we have a material this iteration, add it to the current root joint.
        else if (currentMaterial != nullptr)
            root->AddMaterial(currentMaterial);
        // If we have a shape this iteration, assign it to the last material we added to the current root joint.
        // Also generate shaders, since now that it has a shape the material has all the data it needs.
        else if (currentShape != nullptr) {
            J3DMaterial* shapeMaterial = root->GetLastMaterial();

            shapeMaterial->SetShape(currentShape);
            shapeMaterial->GenerateShaders(mJoints.size());
            J3DUniformBufferObject::LinkMaterialToUBO(shapeMaterial);

            currentShape->ConcatenatePacketsToIBO(&mGXVertices);
        }
    }
}

void J3DModelData::ConvertGXVerticesToGL() {
    std::vector<J3DVertexGX> uniqueGXVerts;

    for (auto a : mGXVertices) {
        ptrdiff_t index = -1;
        bool isInUniqueVec = J3DUtility::VectorContains(uniqueGXVerts, a, index);

        if (!isInUniqueVec) {
            index = uniqueGXVerts.size();
            uniqueGXVerts.push_back(a);

            J3DVertexGL newGLVert = mVertexData.CreateGLVertFromGXVert(a);
            newGLVert.Position.w = a.DrawIndex;

            mGLVertices.push_back(newGLVert);
        }
        
        mIndices.push_back(index);
    }
}

bool J3DModelData::InitializeGL() {
    ConvertGXVerticesToGL();

    // Create VBO
    glCreateBuffers(1, &mVBO);
    glNamedBufferStorage(mVBO, mGLVertices.size() * sizeof(J3DVertexGL), mGLVertices.data(), GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);

    // Create IBO
    glCreateBuffers(1, &mIBO);
    glNamedBufferStorage(mIBO, mIndices.size() * sizeof(uint16_t), mIndices.data(), GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);

    // Create VAO
    glCreateVertexArrays(1, &mVAO);
    if (mVAO == UINT32_MAX)
        return false;

    // Set VBO as the data source for the VAO
    glVertexArrayVertexBuffer(mVAO, 0, mVBO, 0, sizeof(J3DVertexGL));
    // Set IBO as the index source for the VAO
    glVertexArrayElementBuffer(mVAO, mIBO);

    // Configure position data on VAO
    if (mVertexData.HasPositionData()) {
        uint32_t posEnumVal = J3DUtility::EnumToIntegral(EGLAttribute::Position);
        glEnableVertexArrayAttrib(mVAO, posEnumVal);

        glVertexArrayAttribBinding(mVAO, posEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, posEnumVal, glm::vec4::length(), GL_FLOAT, GL_FALSE, offsetof(J3DVertexGL, Position));
    }

    // Configure normal data on VAO
    if (mVertexData.HasNormalData()) {
        uint32_t nrmEnumVal = J3DUtility::EnumToIntegral(EGLAttribute::Normal);
        glEnableVertexArrayAttrib(mVAO, nrmEnumVal);

        glVertexArrayAttribBinding(mVAO, nrmEnumVal, 0);
        glVertexArrayAttribFormat(mVAO, nrmEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(J3DVertexGL, Normal));
    }

    // Configure vertex color data on VAO
    for (int i = 0; i < 2; i++) {
        if (mVertexData.HasColorData(i)) {
            uint32_t colEnumVal = J3DUtility::EnumToIntegral(EGLAttribute::Color0) + i;
            glEnableVertexArrayAttrib(mVAO, colEnumVal);

            glVertexArrayAttribBinding(mVAO, colEnumVal, 0);
            glVertexArrayAttribFormat(mVAO, colEnumVal, glm::vec4::length(), GL_FLOAT, GL_FALSE, offsetof(J3DVertexGL, Color[i]));
        }
    }

    // Configure tex coord data on VAO
    for (int i = 0; i < 8; i++) {
        if (mVertexData.HasTexCoordData(i)) {
            uint32_t texEnumVal = J3DUtility::EnumToIntegral(EGLAttribute::TexCoord0) + i;
            glEnableVertexArrayAttrib(mVAO, texEnumVal);

            glVertexArrayAttribBinding(mVAO, texEnumVal, 0);
            glVertexArrayAttribFormat(mVAO, texEnumVal, glm::vec3::length(), GL_FLOAT, GL_FALSE, offsetof(J3DVertexGL, TexCoord[i]));
        }
    }

    return true;
}

void J3DModelData::Render(float deltaTime) {
    if (!mGLInitialized)
        mGLInitialized = InitializeGL();

    for (int i = 0; i < mEnvelopeIndices.size(); i++) {
        if (mDrawBools[i] == false) {
            EnvelopeMatrices[i] = mJoints[mEnvelopeIndices[i]]->GetTransformMatrix();
        }
        else {
            EnvelopeMatrices[i] = glm::zero<glm::mat4>();

            J3DEnvelope env = mJointEnvelopes[mEnvelopeIndices[i]];

            for (int j = 0; j < env.Weights.size(); j++) {
                uint32_t jointIndex = env.JointIndices[j];

                glm::mat4 ibm = mInverseBindMatrices[jointIndex];
                glm::mat4 jointTransform = mJoints[jointIndex]->GetTransformMatrix();

                EnvelopeMatrices[i] += (jointTransform * ibm) * env.Weights[j];
            }
        }
    }

    J3DUniformBufferObject::SetEnvelopeMatrices(EnvelopeMatrices);

    glBindVertexArray(mVAO);

    mRootJoint->RenderRecursive();

    glBindVertexArray(0);
}
