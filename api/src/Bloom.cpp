#include "Renderer/Shader.hpp"
#include <Renderer/Bloom.hpp>
#include <iostream>

namespace VaultRenderer {
    float quadVertices[] = {
        // upper-left triangle
        -1.0f, -1.0f, 0.0f, 0.0f, // position, texcoord
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        // lower-right triangle
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f};
    unsigned int quadVBO, quadVAO;

    void configureQuad() {
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // texture coordinate
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              (void *)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    bool BloomFBO::Init(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength) {
        if (mInit) return true;

        glGenFramebuffers(1, &mFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

        glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
        glm::ivec2 mipIntSize((int)windowWidth, (int)windowHeight);
        // Safety check
        if (windowWidth > (unsigned int)INT_MAX || windowHeight > (unsigned int)INT_MAX) {
            std::cerr << "Window size conversion overflow - cannot build bloom FBO!\n";
            return false;
        }

        for (unsigned int i = 0; i < mipChainLength; i++) {
            BloomMip mip;

            mipSize *= 0.5f;
            mipIntSize /= 2;
            mip.size = mipSize;
            mip.intSize = mipIntSize;

            glGenTextures(1, &mip.texture);
            glBindTexture(GL_TEXTURE_2D, mip.texture);
            // we are downscaling an HDR color buffer, so we need a float texture format
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
                         (int)mipSize.x, (int)mipSize.y,
                         0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            mMipChain.emplace_back(mip);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, mMipChain[0].texture, 0);

        // setup attachments
        unsigned int attachments[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, attachments);

        // check completion status
        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            printf("gbuffer FBO error, status: 0x\%x\n", status);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        mInit = true;
        return true;
    }

    BloomFBO::BloomFBO() : mInit(false) {}
    BloomFBO::~BloomFBO() {}

    void BloomFBO::Destroy() {
        for (int i = 0; i < mMipChain.size(); i++) {
            glDeleteTextures(1, &mMipChain[i].texture);
            mMipChain[i].texture = 0;
        }
        glDeleteFramebuffers(1, &mFBO);
        mFBO = 0;
        mInit = false;
    }

    void BloomFBO::BindForWriting() {
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    }

    const std::vector<BloomMip> &BloomFBO::MipChain() const {
        return mMipChain;
    }

    bool BloomRenderer::Init(unsigned int windowWidth, unsigned int windowHeight) {
        if (mInit) return true;
        mSrcViewportSize = glm::ivec2(windowWidth, windowHeight);
        mSrcViewportSizeFloat = glm::vec2((float)windowWidth, (float)windowHeight);

        // Framebuffer
        const unsigned int num_bloom_mips = 5; // Experiment with this value
        bool status = mFBO.Init(windowWidth, windowHeight, num_bloom_mips);
        if (!status) {
            std::cerr << "Failed to initialize bloom FBO - cannot create bloom renderer!\n";
            return false;
        }

        // Shaders
        mDownsampleShader = new Shader("./shaders/downsample.glsl");
        mUpsampleShader = new Shader("./shaders/upsample.glsl");

        // Downsample
        mDownsampleShader->Bind();
        mDownsampleShader->SetUniform1i("srcTexture", 0);
        mDownsampleShader->Unbind();

        // Upsample
        mUpsampleShader->Bind();
        mUpsampleShader->SetUniform1i("srcTexture", 0);
        mUpsampleShader->Unbind();

        mInit = true;
        return true;
    }

    BloomRenderer::BloomRenderer() : mInit(false) {
        configureQuad();
    }
    BloomRenderer::~BloomRenderer() {}

    void BloomRenderer::Destroy() {
        mFBO.Destroy();
        delete mDownsampleShader;
        delete mUpsampleShader;
        mInit = false;
    }

    void BloomRenderer::RenderBloomTexture(unsigned int srcTexture, float filterRadius) {
        mFBO.BindForWriting();

        this->RenderDownsamples(srcTexture);
        this->RenderUpsamples(filterRadius);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Restore viewport
        glViewport(0, 0, mSrcViewportSize.x, mSrcViewportSize.y);
    }

    GLuint BloomRenderer::BloomTexture() {
        return mFBO.MipChain()[0].texture;
    }

    void BloomRenderer::RenderDownsamples(unsigned int srcTexture) {
        const std::vector<BloomMip> &mipChain = mFBO.MipChain();

        mDownsampleShader->Bind();
        mDownsampleShader->SetUniform2f("srcResolution", mSrcViewportSizeFloat.x, mSrcViewportSizeFloat.y);

        // Bind srcTexture (HDR color buffer) as initial texture input
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, srcTexture);

        // Progressively downsample through the mip chain
        for (int i = 0; i < mipChain.size(); i++) {
            const BloomMip &mip = mipChain[i];
            glViewport(0, 0, mip.size.x, mip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, mip.texture, 0);

            // Render screen-filled quad of resolution of current mip
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            // Set current mip resolution as srcResolution for next iteration
            mDownsampleShader->SetUniform2f("srcResolution", mip.size.x, mip.size.y);
            // Set current mip as texture input for next iteration
            glBindTexture(GL_TEXTURE_2D, mip.texture);
        }

        mDownsampleShader->Unbind();
    }

    void BloomRenderer::RenderUpsamples(float filterRadius) {
        const std::vector<BloomMip> &mipChain = mFBO.MipChain();

        mUpsampleShader->Bind();
        mUpsampleShader->SetUniform1f("filterRadius", filterRadius);

        // Enable additive blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        for (int i = mipChain.size() - 1; i > 0; i--) {
            const BloomMip &mip = mipChain[i];
            const BloomMip &nextMip = mipChain[i - 1];

            // Bind viewport and texture from where to read
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mip.texture);

            // Set framebuffer render target (we write to this texture)
            glViewport(0, 0, nextMip.size.x, nextMip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, nextMip.texture, 0);

            // Render screen-filled quad of resolution of current mip
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        // Disable additive blending
        // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was default
        glDisable(GL_BLEND);

        mUpsampleShader->Unbind();
    }
} // namespace VaultRenderer