#include "J3D/J3DModelData.hpp"
#include "J3D/J3DNode.hpp"

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
        }
    }
}
