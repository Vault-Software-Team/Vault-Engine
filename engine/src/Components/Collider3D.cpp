#include "Engine/Components/Collider3D.hpp"
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void Collider3D::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void BoxCollider3D::OnGUI() {
        DrawComponent<BoxCollider3D>(Scene::Main->EntityRegistry, 40, [&] {
            Editor::GUI::DrawVec3Control("Size", size);
        });
    }

    void MeshCollider3D::OnGUI() {
        DrawComponent<MeshCollider3D>(Scene::Main->EntityRegistry, 40, [&] {
            Editor::GUI::DrawVec3Control("Size", size);
        });
    }
} // namespace Engine::Components
  // namespace Engine::Components