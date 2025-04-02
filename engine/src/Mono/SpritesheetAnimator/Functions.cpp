#include "Engine/Components/SpritesheetAnimator.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Mono/HelperFunctions.hpp"
#include <Engine/Mono/SpritesheetAnimator/Functions.hpp>
#include <Editor/GUI/MainGUI.hpp>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *SpritesheetAnimator_GetTexture(MonoString *ms_ID) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return CSharpHelper::StrToMonoString("");
            }

            auto &spritesheetAnimator = gameObject->GetComponent<Components::SpritesheetAnimator>();
            return CSharpHelper::StrToMonoString(spritesheetAnimator.mesh->material.diffuse != nullptr ? spritesheetAnimator.mesh->material.diffuse->texture_data->texture_filepath : "null");
        }

        MonoString *SpritesheetAnimator_GetColor(MonoString *ms_ID) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return 0;
            }

            auto &spritesheetAnimator = gameObject->GetComponent<Components::SpritesheetAnimator>();
            return CSharpHelper::StrToMonoString(CSharpHelper::Vec3ToStr(spritesheetAnimator.mesh->material.color));
        }

        MonoString *SpritesheetAnimator_GetCurrentAnimation(MonoString *ms_ID) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return 0;
            }

            auto &spritesheetAnimator = gameObject->GetComponent<Components::SpritesheetAnimator>();
            return CSharpHelper::StrToMonoString(spritesheetAnimator.current_animation);
        }

        bool SpritesheetAnimator_GetAnimate(MonoString *ms_ID) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return 0;
            }

            auto &spritesheetAnimator = gameObject->GetComponent<Components::SpritesheetAnimator>();
            return spritesheetAnimator.animate;
        }

        void SpritesheetAnimator_SetTexture(MonoString *ms_ID, MonoString *ms_texture) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);
            const std::string content = CSharpHelper::MonoStrToString(ms_texture);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return;
            }

            auto &spritesheetAnimator = gameObject->GetComponent<Components::SpritesheetAnimator>();
            spritesheetAnimator.mesh->material.SetDiffuse(content);
        }

        void SpritesheetAnimator_SetColor(MonoString *ms_ID, float r, float g, float b) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return;
            }

            auto &spritesheetAnimator = gameObject->GetComponent<Components::SpritesheetAnimator>();
            spritesheetAnimator.mesh->material.color.r = r;
            spritesheetAnimator.mesh->material.color.g = g;
            spritesheetAnimator.mesh->material.color.b = b;
        }

        void SpritesheetAnimator_SetCurrentAnimation(MonoString *ms_ID, MonoString *ms_current_animation) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return;
            }

            auto &spritesheetAnimator = gameObject->GetComponent<Components::SpritesheetAnimator>();
            spritesheetAnimator.current_animation = CSharpHelper::MonoStrToString(ms_current_animation);
        }

        void SpritesheetAnimator_SetAnimate(MonoString *ms_ID, bool ms_animate) {
            const std::string id = CSharpHelper::MonoStrToString(ms_ID);

            auto gameObject = GameObject::FindGameObjectByID(id);

            if (!gameObject) {
                Editor::GUI::LogError("C# Error: GameObject not found!\n");
                return;
            }

            auto &spritesheetAnimator = gameObject->GetComponent<Components::SpritesheetAnimator>();
            spritesheetAnimator.animate = ms_animate;
        }

    } // namespace CSharpInternalFunctions
} // namespace Engine