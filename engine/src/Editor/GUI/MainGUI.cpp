#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>

using namespace Engine::Components;

namespace Editor {
    void GUI::SetNameIcon(std::string &txt, std::shared_ptr<Engine::GameObject> &gameObject) {
        if (gameObject->HasComponent<Camera>()) {
            txt = ICON_FA_CAMERA;
        }

        if (gameObject->HasComponent<AmbientLight>()) {
            txt = ICON_FA_STAR;
        }

        if (gameObject->HasComponent<DirectionalLight>()) {
            txt = ICON_FA_SUN;
        }

        if (gameObject->HasComponent<PointLight>()) {
            txt = ICON_FA_LIGHTBULB;
        }

        if (gameObject->HasComponent<Text3D>()) {
            txt = ICON_FA_FONT;
        }
    }
} // namespace Editor