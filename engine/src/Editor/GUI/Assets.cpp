#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <filesystem>

using namespace Engine;
using namespace Engine::Components;

char asciitolower(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

void DirectoryIterator(const std::string &str, const char *filter_str) {
    auto iter = std::filesystem::directory_iterator(str);

    std::string filterStr = filter_str;
    std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), asciitolower);

    for (auto &dir : iter) {

        if (dir.is_directory()) {
            if (ImGui::TreeNode((std::string(ICON_FA_FOLDER) + " " + dir.path().filename().string()).c_str())) {
                DirectoryIterator(dir.path().string(), filter_str);
                ImGui::TreePop();
            }
        } else {
            std::string dir_filename = dir.path().filename().string();
            std::transform(dir_filename.begin(), dir_filename.end(), dir_filename.begin(), asciitolower);
            if (dir_filename.find(filterStr) == std::string::npos && filterStr != "")
                continue;
            std::string icon = ICON_FA_CUBE;
            std::string name = dir.path().filename();

            if (name.ends_with(".ttf")) {
                icon = ICON_FA_FONT;
            }
            if (name.ends_with(".png") || name.ends_with(".tiff") || name.ends_with(".jpg") || name.ends_with(".jpeg")) {
                icon = ICON_FA_IMAGE;
            }
            if (name.ends_with(".obj")) {
                icon = ICON_FA_CUBES;
            }
            if (name.ends_with(".mtl")) {
                icon = ICON_FA_PAINT_ROLLER;
            }

            ImGui::Selectable((icon + " " + dir.path().filename().string()).c_str());
        }
    }
}

namespace Editor {
    void GUI::Assets() {
        constexpr int filter_str_size = 256;
        static char filter_str[filter_str_size];

        ImGui::Begin(ICON_FA_FOLDER " Assets");
        ImGui::PushItemWidth(ImGui::GetWindowSize().x - 20);
        ImGui::InputText("##AssetsFilterString", filter_str, filter_str_size);
        std::string icon = ICON_FA_CUBE;

        DirectoryIterator("../assets", filter_str);

        ImGui::End();
    }
} // namespace Editor