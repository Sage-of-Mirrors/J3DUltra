#include "J3D/Picking/J3DPicking.hpp"

#include <glad/glad.h>


namespace J3D {
    namespace Picking {
        namespace {
            constexpr uint32_t DATA_RESET = 0;
            constexpr float DEPTH_RESET = 1.0f;

            constexpr int TEX_DATA = 0;
            constexpr int TEX_DEPTH = 1;

            uint32_t mWidth;
            uint32_t mHeight;

            uint32_t mFBO = 0;
            uint32_t mTexObjs[2] = { 0, 0 };

            uint32_t mPickingShaderId = 0;
        }

        bool IsPickingEnabled() {
            return mFBO != 0;
        }

        void InitFramebuffer(uint32_t width, uint32_t height) {
            if (IsPickingEnabled()) {
                DestroyFramebuffer();
            }

            mWidth = width;
            mHeight = height;

            // Generate framebuffer
            glCreateFramebuffers(1, &mFBO);

            // Generate data texture
            glCreateTextures(GL_TEXTURE_2D, 2, mTexObjs);
            glTextureStorage2D(mTexObjs[TEX_DATA], 1, GL_R32UI, width, height);
            glTextureParameteri(mTexObjs[TEX_DATA], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(mTexObjs[TEX_DATA], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // Generate depth texture
            glTextureStorage2D(mTexObjs[TEX_DEPTH], 1, GL_DEPTH_COMPONENT32F, width, height);

            // Attach textures to framebuffer
            glNamedFramebufferTexture(mFBO, GL_COLOR_ATTACHMENT0, mTexObjs[TEX_DATA], 0);
            glNamedFramebufferTexture(mFBO, GL_DEPTH_ATTACHMENT, mTexObjs[TEX_DEPTH], 0);
        }

        void ResizeFramebuffer(uint32_t width, uint32_t height) {
            InitFramebuffer(width, height);
        }

        void DestroyFramebuffer() {
            if (!IsPickingEnabled()) {
                return;
            }

            glDeleteFramebuffers(1, &mFBO);
            glDeleteTextures(2, mTexObjs);
        }

        void RenderPickingScene(glm::mat4& viewMatrix, glm::mat4& projMatrix, J3DRendering::RenderFunctionArgs modelInstances) {
            if (!IsPickingEnabled()) {
                return;
            }

            // Bind FBO
            glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
            glViewport(0, 0, mWidth, mHeight);

            glClearBufferuiv(GL_COLOR, 0, &DATA_RESET);
            glClearBufferfv(GL_DEPTH, 0, &DEPTH_RESET);

            J3DRendering::Render(0, glm::vec3(), viewMatrix, projMatrix, modelInstances, mPickingShaderId);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        ModelMaterialIdPair Query(uint32_t pX, uint32_t pY) {
            if (!IsPickingEnabled()) {
                return ModelMaterialIdPair(0, 0);
            }

            uint32_t pixelValue;
            glReadPixels(pX, pY, 1, 1, GL_R32UI, GL_UNSIGNED_INT, &pixelValue);

            uint16_t modelId = (pixelValue & 0xFFFF0000) >> 0x10;
            uint16_t matId = pixelValue & 0x0000FFFF;

            return ModelMaterialIdPair(modelId, matId);
        }

        bool Query(uint32_t pX, uint32_t pY, const J3DModelInstance& model, const J3DMaterial* material) {
            if (!IsPickingEnabled()) {
                return false;
            }

            uint32_t pixelValue;
            glReadPixels(pX, pY, 1, 1, GL_R32UI, GL_UNSIGNED_INT, &pixelValue);

            uint16_t modelId = (pixelValue & 0xFFFF0000) >> 0x10;
            uint16_t matId = pixelValue & 0x0000FFFF;

            // TODO: compare model and material ids

            return false;
        }
    }
}