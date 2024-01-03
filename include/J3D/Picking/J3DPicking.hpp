#pragma once

#include "J3D/Rendering/J3DRendering.hpp"

#include <cstdint>
#include <tuple>


class J3DModelInstance;
class J3DMaterial;

namespace J3D {
    namespace Picking {
        using ModelMaterialIdPair = std::tuple<uint16_t, uint16_t>;

        /// <summary>
        /// Returns whether viewport picking in J3DUltra is enabled.
        /// </summary>
        /// <returns>Whether picking is enabled</returns>
        bool IsPickingEnabled();

        /// <summary>
        /// Initializes the picking framebuffer with the given width and height. Be sure
        /// to call DestroyFramebuffer() when finished with picking!
        /// </summary>
        /// <param name="width">Width of the framebuffer, in pixels</param>
        /// <param name="height">Height of the framebuffer, in pixels</param>
        void InitFramebuffer(uint32_t width, uint32_t height);
        /// <summary>
        /// Sets the size of the picking framebuffer that will be used to store the result
        /// of a picking render pass.
        /// </summary>
        /// <param name="width">Width of the framebuffer, in pixels</param>
        /// <param name="height">Height of the framebuffer, in pixels</param>
        void ResizeFramebuffer(uint32_t width, uint32_t height);
        /// <summary>
        /// Frees all GL resources allocated by InitFramebuffer.
        /// </summary>
        void DestroyFramebuffer();

        /// <summary>
        /// Renders a picking scene with the given View and Projection matrices to the picking framebuffer.
        /// </summary>
        /// <param name="viewMatrix">View matrix to render the picking scene with</param>
        /// <param name="projMatrix">Projection matrix to render the picking scene with</param>
        /// <param name="modelInstances">Model instances to render in the picking scene</param>
        void RenderPickingScene(glm::mat4& viewMatrix, glm::mat4& projMatrix, J3DRendering::RenderFunctionArgs modelInstances);

        /// <summary>
        /// Returns a tuple of (Model Id, Material Id) from the picking framebuffer at
        /// the given X and Y pixel coordinates. If no model is present at those coordinates,
        /// the tuple contains (0, 0).
        /// </summary>
        /// <param name="pX">X coordinate of the pixel to examine</param>
        /// <param name="pY">Y coordinate of the pixel to examine</param>
        /// <returns>A tuple of (Model Id, Material Id), or (0, 0) if no model was present at the given coordinates</returns>
        ModelMaterialIdPair Query(uint32_t pX, uint32_t pY);
        /// <summary>
        /// Returns whether the given model instance, and optionally a specific material, were
        /// found in the picking framebuffer at the given X and Y pixel coordinates. If
        /// <paramref name="material"/> is nullptr, only the model is considered.
        /// </summary>
        /// <param name="pX">X coordinate of the pixel to examine</param>
        /// <param name="pY">Y coordinate of the pixel to examine</param>
        /// <param name="model">Model instance to check for in the picking framebuffer</param>
        /// <param name="material">Material to check for in the picking framebuffer</param>
        /// <returns>Whether the given model instance (and optionally material) were present at the given coordinates</returns>
        bool Query(uint32_t pX, uint32_t pY, const J3DModelInstance& model, const J3DMaterial* material);
    }
}