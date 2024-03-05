#include <Renderer/Bloom.hpp>
#include <iostream>

namespace VaultRenderer {
    bool BloomBuffer::Init(uint32_t windowWidth, uint32_t windowHeight,
                           uint32_t mipChainLength) {
        if (m_Init)
            return true;

        glGenFramebuffers(1, &m_Framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

        glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
        glm::ivec2 mipIntSize((int)windowWidth, (int)windowHeight);
        // Safety check
        if (windowWidth > (uint32_t)INT_MAX ||
            windowHeight > (uint32_t)INT_MAX) {
            std::cerr << "Window size conversion overflow - cannot build "
                         "bloom FBO!\n";
            return false;
        }

        for (uint32_t i = 0; i < mipChainLength; i++) {
            BloomMip mip;

            mipSize *= 0.5f;
            mipIntSize /= 2;
            mip.size = mipSize;
            mip.intSize = mipIntSize;

            glGenTextures(1, &mip.texture);
            glBindTexture(GL_TEXTURE_2D, mip.texture);
            // we are downscaling an HDR color buffer, so we need a float
            // texture format
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x,
                         (int)mipSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            m_MipChain.emplace_back(mip);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_MipChain[0].texture, 0);

        // setup attachments
        uint32_t attachments[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, attachments);

        // check completion status
        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Bloom FBO is not complete!\n";
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_Init = true;
        return true;
    }

    BloomBuffer::BloomBuffer() : m_Init(false) {}
    BloomBuffer::~BloomBuffer() {}

    void BloomBuffer::Destroy() {
        for (int i = 0; i < m_MipChain.size(); i++) {
            glDeleteTextures(1, &m_MipChain[i].texture);
            m_MipChain[i].texture = 0;
        }
        glDeleteFramebuffers(1, &m_Framebuffer);
        m_Framebuffer = 0;
        m_Init = false;
    }

    void BloomBuffer::BindWriting() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    }

    bool BloomRenderer::Init(uint32_t windowWidth, uint32_t windowHeight) {
        if (mInit)
            return true;
        m_SrcViewportSize = glm::ivec2(windowWidth, windowHeight);
        m_SrcViewportSizeFloat =
            glm::vec2((float)windowWidth, (float)windowHeight);

        // Framebuffer
        const uint32_t num_bloom_mips = 5; // Experiment with this value
        bool status =
            m_Framebuffer.Init(windowWidth, windowHeight, num_bloom_mips);
        if (!status) {
            std::cerr << "Failed to initialize bloom FBO - cannot create bloom "
                         "renderer!\n";
            return false;
        }

        // Shaders
        m_DownsampleShader = new Shader("./shaders/downsample.glsl");
        m_UpsampleShader = new Shader("./shaders/upsample.glsl");

        // Downsample
        m_DownsampleShader->Bind();
        m_DownsampleShader->SetUniform1i("srcTexture", 0);

        // Upsample
        m_UpsampleShader->Bind();
        m_UpsampleShader->SetUniform1i("srcTexture", 0);

        mInit = true;
        return true;
    }

    BloomRenderer::BloomRenderer() : mInit(false) {}
    BloomRenderer::~BloomRenderer() {}

    void BloomRenderer::Destroy() {
        m_Framebuffer.Destroy();
        delete m_DownsampleShader;
        delete m_UpsampleShader;
        mInit = false;
    }

    void BloomRenderer::RenderBloomTexture(uint32_t srcTexture,
                                           float filterRadius,
                                           uint32_t &quadVAO) {
        m_Framebuffer.BindWriting();

        this->RenderDownsamples(srcTexture, quadVAO);
        this->RenderUpsamples(filterRadius, quadVAO);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Restore viewport
        glViewport(0, 0, m_SrcViewportSize.x, m_SrcViewportSize.y);
    }

    uint32_t BloomRenderer::BloomTexture() {
        return m_Framebuffer.GetMipChain()[0].texture;
    }

    void BloomRenderer::RenderDownsamples(uint32_t srcTexture,
                                          uint32_t &quadVAO) {
        const std::vector<BloomMip> &mipChain = m_Framebuffer.GetMipChain();

        m_DownsampleShader->Bind();
        m_DownsampleShader->SetUniform2f("srcResolution",
                                         m_SrcViewportSizeFloat.x,
                                         m_SrcViewportSizeFloat.y);

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

            // Set current mip resolution as srcResolution for next
            // iteration
            m_DownsampleShader->SetUniform2f("srcResolution", mip.size.x,
                                             mip.size.y);
            // Set current mip as texture input for next iteration
            glBindTexture(GL_TEXTURE_2D, mip.texture);
        }
    }

    void BloomRenderer::RenderUpsamples(float filterRadius, uint32_t &quadVAO) {
        const std::vector<BloomMip> &mipChain = m_Framebuffer.GetMipChain();

        m_UpsampleShader->Bind();
        m_DownsampleShader->SetUniform1f("filterRadius", filterRadius);

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
        // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this
        // was default
        glDisable(GL_BLEND);
    }
} // namespace VaultRenderer