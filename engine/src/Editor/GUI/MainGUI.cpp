#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>

using namespace Engine::Components;

namespace Editor {
    DLL_API std::vector<Log> GUI::logs;
    DLL_API uint32_t GUI::framebufferTextureID;
    DLL_API Engine::GameObject *GUI::selected_gameObject;

    char asciitolower(char in) {
        if (in <= 'Z' && in >= 'A')
            return in - ('Z' - 'z');
        return in;
    }

    void GUI::SetNameIcon(std::string &txt, Engine::GameObject *gameObject) {
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