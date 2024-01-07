#include <Engine/Components/AmbientLight.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void AmbientLight::AttachToShader(VaultRenderer::Shader &shader) {
        static int before_amount;
        static glm::vec3 before_color;

        if (before_amount != amount || before_color != color) {
            shader.Bind();
            shader.SetUniform1f("ambient_amount", amount);
            shader.SetUniform3f("ambient_color", color.x, color.y, color.z);
        }

        before_amount = amount;
        before_color = color;
    }

    void AmbientLight::OnGUI() {
        DrawComponent<AmbientLight>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::DragFloat("Amount", &amount, 0.01f, 0.0);
        });
    }
} // namespace Engine::Components
  // namespace Engine::Components