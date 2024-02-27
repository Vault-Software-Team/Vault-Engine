#include "Engine/Components/Text3D.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Mono/HelperFunctions.hpp"
#include <Engine/Mono/Text3D/Functions.hpp>
#include <Editor/GUI/MainGUI.hpp>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *Text3D_GetText(MonoString *ms_ID) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return CSharpHelper::StrToMonoString("");
            }

            auto &text3D = gameObject->GetComponent<Components::Text3D>();
            return CSharpHelper::StrToMonoString(text3D.text);
        }
        float Text3D_GetScale(MonoString *ms_ID) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return 0;
            }

            auto &text3D = gameObject->GetComponent<Components::Text3D>();
            return text3D.scale;
        }
        float Text3D_GetYOffset(MonoString *ms_ID) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return 0;
            }

            auto &text3D = gameObject->GetComponent<Components::Text3D>();
            return text3D.y_offset;
        }
        MonoString *Text3D_GetColor(MonoString *ms_ID) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return 0;
            }

            auto &text3D = gameObject->GetComponent<Components::Text3D>();
            return CSharpHelper::StrToMonoString(CSharpHelper::Vec3ToStr(text3D.color));
        }

        void Text3D_SetText(MonoString *ms_ID, MonoString *ms_content) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);
            const std::string content = CSharpHelper::MonoStrToString(ms_content);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return;
            }

            auto &text3D = gameObject->GetComponent<Components::Text3D>();
            text3D.text = content;
        }
        void Text3D_SetScale(MonoString *ms_ID, float val) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return;
            }

            auto &text3D = gameObject->GetComponent<Components::Text3D>();
            text3D.scale = val;
        }
        void Text3D_SetYOffset(MonoString *ms_ID, float val) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return;
            }

            auto &text3D = gameObject->GetComponent<Components::Text3D>();
            text3D.y_offset = val;
        }
        void Text3D_SetColor(MonoString *ms_ID, float r, float g, float b) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return;
            }

            auto &text3D = gameObject->GetComponent<Components::Text3D>();
            text3D.color.r = r;
            text3D.color.g = g;
            text3D.color.b = b;
        }

    } // namespace CSharpInternalFunctions
} // namespace Engine