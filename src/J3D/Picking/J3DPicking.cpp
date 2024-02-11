#include "J3D/Picking/J3DPicking.hpp"
#include "J3D/Picking/J3DPickingShaders.hpp"
#include "J3D/Data/J3DModelInstance.hpp"
#include "J3D/Material/J3DMaterial.hpp"
#include "J3D/Material/J3DUniformBufferObject.hpp"

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

            if (mPickingShaderId == 0) {
                int32_t vertShader = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertShader, 1, &VtxShader, NULL);
                glCompileShader(vertShader);

                int32_t fragShader = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragShader, 1, &FragShader, NULL);
                glCompileShader(fragShader);

                mPickingShaderId = glCreateProgram();
                glAttachShader(mPickingShaderId, vertShader);
                glAttachShader(mPickingShaderId, fragShader);

                glLinkProgram(mPickingShaderId);
                J3DUniformBufferObject::LinkShaderProgramToUBO(mPickingShaderId);

                glDetachShader(mPickingShaderId, vertShader);
                glDetachShader(mPickingShaderId, fragShader);
                glDeleteShader(vertShader);
                glDeleteShader(fragShader);
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

        void RenderPickingScene(glm::mat4& viewMatrix, glm::mat4& projMatrix, J3D::Rendering::RenderPacketVector& renderPackets) {
            if (!IsPickingEnabled()) {
                return;
            }

            // Bind FBO
            glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
            glViewport(0, 0, mWidth, mHeight);

            glDepthMask(true);
            glClearBufferuiv(GL_COLOR, 0, &DATA_RESET);
            glClearBufferfv(GL_DEPTH, 0, &DEPTH_RESET);

            J3D::Rendering::Render(0, viewMatrix, projMatrix, renderPackets, mPickingShaderId);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        uint32_t ReadPixel(uint32_t pX, uint32_t pY) {
            glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
            glViewport(0, 0, mWidth, mHeight);

            uint32_t pixelValue = 0;
            glReadPixels(pX, pY, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixelValue);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            return pixelValue;
        }

        ModelMaterialIdPair Query(uint32_t pX, uint32_t pY) {
            if (!IsPickingEnabled()) {
                return ModelMaterialIdPair(0, 0);
            }

            uint32_t pixelValue = ReadPixel(pX, pY);

            uint16_t modelId = (pixelValue & 0xFFFF0000) >> 0x10;
            uint16_t matId = pixelValue & 0x0000FFFF;

            return ModelMaterialIdPair(modelId, matId);
        }

        bool Query(uint32_t pX, uint32_t pY, const J3DModelInstance& model, const J3DMaterial* material) {
            if (!IsPickingEnabled()) {
                return false;
            }

            uint32_t pixelValue = ReadPixel(pX, pY);

            uint16_t modelId = (pixelValue & 0xFFFF0000) >> 0x10;
            uint16_t matId = pixelValue & 0x0000FFFF;

            bool bModelIdMatch = modelId == model.GetModelId();
            bool bMaterialIdMatch = material != nullptr ? matId == material->GetMaterialId() : true;

            return bModelIdMatch && bMaterialIdMatch;
        }
    }
}
