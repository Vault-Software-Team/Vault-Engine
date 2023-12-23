#include <Renderer/ShadowMap.hpp>

namespace VaultRenderer {
    ShadowMap::ShadowMap() {
        shader = std::make_unique<Shader>("../shaders/shadow_map.glsl");
    }

    void ShadowMap::RenderSpace(std::function<void(std::unique_ptr<Shader> &)> shadow_render_call) {
        depth_buffer.Bind();
        shader->Bind();
        shader->SetUniformMat4("light_proj", light_proj);
        shadow_render_call(shader);
        depth_buffer.Unbind();
    }

    void ShadowMap::ChangeShadowMapSize(const uint32_t width, const uint32_t height) {
        depth_buffer.width = width;
        depth_buffer.height = height;
        depth_buffer.RegenerateFramebuffer();
    }

    void ShadowMap::CalculateMatrices(const glm::vec3 &light_position) {
        ortho_proj = glm::ortho(-ortho_size, ortho_size, -ortho_size, ortho_size, near, far);
        light_view = glm::lookAt(light_position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        light_proj = ortho_proj * light_view;
    }

    void ShadowMap::BindTexture(int slot) {
        depth_buffer.BindTexture(slot);
    }

    uint32_t &ShadowMap::GetTextureID() {
        return depth_buffer.texture;
    }

    DepthFramebuffer &ShadowMap::GetDepthBuffer() {
        return depth_buffer;
    }

    void ShadowMap::SetLightProjection(Shader &shader) {
        shader.Bind();
        shader.SetUniformMat4("light_proj", light_proj);
    }
} // namespace VaultRenderer