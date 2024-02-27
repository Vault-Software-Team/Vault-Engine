#include "Engine/Components/SpriteRenderer.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Mono/HelperFunctions.hpp"
#include <Engine/Mono/SpriteRenderer/Functions.hpp>
#include <Editor/GUI/MainGUI.hpp>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *SpriteRenderer_GetTexture(MonoString *ms_ID) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return CSharpHelper::StrToMonoString("");
            }

            auto &spriteRenderer = gameObject->GetComponent<Components::SpriteRenderer>();
            return CSharpHelper::StrToMonoString(spriteRenderer.mesh->material.diffuse != nullptr ? spriteRenderer.mesh->material.diffuse->texture_data->texture_filepath : "null");
        }

        MonoString *SpriteRenderer_GetColor(MonoString *ms_ID) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return 0;
            }

            auto &spriteRenderer = gameObject->GetComponent<Components::SpriteRenderer>();
            return CSharpHelper::StrToMonoString(CSharpHelper::Vec3ToStr(spriteRenderer.mesh->material.color));
        }

        void SpriteRenderer_SetTexture(MonoString *ms_ID, MonoString *ms_texture) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);
            const std::string content = CSharpHelper::MonoStrToString(ms_texture);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return;
            }

            auto &spriteRenderer = gameObject->GetComponent<Components::SpriteRenderer>();
            spriteRenderer.mesh->material.SetDiffuse(content);
        }

        void SpriteRenderer_SetColor(MonoString *ms_ID, float r, float g, float b) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return;
            }

            auto &spriteRenderer = gameObject->GetComponent<Components::SpriteRenderer>();
            spriteRenderer.mesh->material.color.r = r;
            spriteRenderer.mesh->material.color.g = g;
            spriteRenderer.mesh->material.color.b = b;
        }

    } // namespace CSharpInternalFunctions
} // namespace Engine