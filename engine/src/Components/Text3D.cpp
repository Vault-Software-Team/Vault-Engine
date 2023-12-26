#include <Engine/Components/Text3D.hpp>
#include <Engine/Scene.hpp>
#include <iostream>

namespace Engine::Components {
    void Text3D::Draw(VaultRenderer::Shader &shader) {
        transform->UpdateModel();
        font->Draw(shader, transform->model, text, color, glm::vec3(0, 0, 0), 0, 0, scale, y_offset);
    }

    void Text3D::ChangeFont(const std::string &font_path) {
        font.reset();
        font = std::make_unique<VaultRenderer::Font>(font_path.c_str(), 64);
    }

    void Text3D::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }
} // namespace Engine::Components