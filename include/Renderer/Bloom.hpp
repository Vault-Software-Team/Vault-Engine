#pragma once
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <memory>
#include <Renderer/Shader.hpp>

namespace VaultRenderer {
    struct DLL_API BloomMip {
        glm::vec2 size;
        glm::ivec2 intSize;
        unsigned int texture;
    };

    class DLL_API BloomFBO {
    public:
        BloomFBO();
        ~BloomFBO();
        bool Init(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength);
        void Destroy();
        void BindForWriting();
        const std::vector<BloomMip> &MipChain() const;

    private:
        bool mInit;
        unsigned int mFBO;
        std::vector<BloomMip> mMipChain;
    };

    class BloomRenderer {
    public:
        BloomRenderer();
        ~BloomRenderer();
        bool Init(unsigned int windowWidth, unsigned int windowHeight);
        void Destroy();
        void RenderBloomTexture(unsigned int srcTexture, float filterRadius);
        unsigned int BloomTexture();

    public:
        void RenderDownsamples(unsigned int srcTexture);
        void RenderUpsamples(float filterRadius);

        bool mInit;
        BloomFBO mFBO;
        glm::ivec2 mSrcViewportSize;
        glm::vec2 mSrcViewportSizeFloat;
        Shader *mDownsampleShader;
        Shader *mUpsampleShader;
    };
} // namespace VaultRenderer