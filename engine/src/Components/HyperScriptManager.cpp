#include <Engine/Components/HyperScriptManager.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Engine/GameObject.hpp>
#include <icons/icons.h>
#include <Editor/GUI/MainGUI.hpp>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <uuid.hpp>
#include <Engine/Runtime.hpp>

bool replace(std::string &str, const std::string &from, const std::string &to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

namespace Engine::Components {
    void PushArgumentToFunction(HyperScript::Function *func, const std::string &value) {
        func->arguments.push_back(HyperScript::Variable{HyperScript::GT_STRING, value, "content"});
        func->arguments.back().name = func->limitless_args ? uuid::generate_uuid_v4() : func->argument_names[func->arguments.size() - 1];
        // if (func->argument_names.size() > func->arguments.size()) {
        // throw std::runtime_error("Exceeded maximum arguments!");
        // }
    }

    std::string LoadFile(const std::string &path) {
        std::ifstream file(path);
        std::stringstream ss;
        ss << file.rdbuf();
        file.close();
        return ss.str();
    }

    void SetComponentStructures(std::shared_ptr<HyperScript::ScriptEngine> &engine) {
        engine->CreateTemplate("Transform", LoadFile("../assets/HYPERSCRIPT_API/Transform.hyper"));
    }

    // HYPERSCRIPT FUNCTIONS

    void Vault_GetComponent(HyperScript::Function *func) {
        using namespace HyperScript;
        Variable transformObj = func->arguments[0];
        if (transformObj.type != GT_OBJECT || !transformObj.isRef || !transformObj.ref) return;

        auto GameObject_ID = ((ScriptEngine *)func->engine)->GetVariable("GameObject.ID");
        auto &gameObject = GameObject::FindGameObjectByID(GameObject_ID->value);

        std::string comp_name = transformObj.ref->json_handle["component"];

        if (comp_name == "Transform") {
            auto &component = gameObject->GetComponent<Transform>();
            transformObj.ref->json_handle["position"]["x"] = component.position.x;
            transformObj.ref->json_handle["position"]["y"] = component.position.y;
            transformObj.ref->json_handle["position"]["z"] = component.position.z;

            transformObj.ref->json_handle["rotation"]["x"] = component.rotation.x;
            transformObj.ref->json_handle["rotation"]["y"] = component.rotation.y;
            transformObj.ref->json_handle["rotation"]["z"] = component.rotation.z;

            transformObj.ref->json_handle["scale"]["x"] = component.scale.x;
            transformObj.ref->json_handle["scale"]["y"] = component.scale.y;
            transformObj.ref->json_handle["scale"]["z"] = component.scale.z;

            transformObj.ref->value = nlohmann::to_string(transformObj.ref->json_handle);
            transformObj.LoadValueToJSONHandle(func->engine);
        }
    }

    void Vault_UpdateComponent(HyperScript::Function *func) {
        using namespace HyperScript;
        Variable transformObj = func->arguments[0];
        if (transformObj.type != GT_OBJECT || !transformObj.isRef || !transformObj.ref) return;

        auto *engine = (ScriptEngine *)func->engine;

        auto GameObject_ID = engine->GetVariable("GameObject.ID");
        auto &gameObject = GameObject::FindGameObjectByID(GameObject_ID->value);

        std::string comp_name = transformObj.ref->json_handle["component"];

        if (comp_name == "Transform") {
            auto &component = gameObject->GetComponent<Transform>();

            auto pos_x = std::atof(engine->GetVariable(comp_name + "." + "position.x")->value.c_str());
            auto pos_y = std::atof(engine->GetVariable(comp_name + "." + "position.y")->value.c_str());
            auto pos_z = std::atof(engine->GetVariable(comp_name + "." + "position.z")->value.c_str());

            auto rot_x = std::atof(engine->GetVariable(comp_name + "." + "rotation.x")->value.c_str());
            auto rot_y = std::atof(engine->GetVariable(comp_name + "." + "rotation.y")->value.c_str());
            auto rot_z = std::atof(engine->GetVariable(comp_name + "." + "rotation.z")->value.c_str());

            auto scal_x = std::atof(engine->GetVariable(comp_name + "." + "scale.x")->value.c_str());
            auto scal_y = std::atof(engine->GetVariable(comp_name + "." + "scale.y")->value.c_str());
            auto scal_z = std::atof(engine->GetVariable(comp_name + "." + "scale.z")->value.c_str());

            component.position = glm::vec3(pos_x, pos_y, pos_z);
            component.rotation = glm::vec3(rot_x, rot_y, rot_z);
            component.scale = glm::vec3(scal_x, scal_y, scal_z);
        }
    }

    void RegisterFunctions(std::shared_ptr<HyperScript::ScriptEngine> &engine) {
        auto VaultFunc_GetComponent = engine->CreateFunction("Vault.GetComponent", 1, Vault_GetComponent);
        VaultFunc_GetComponent->argument_names.push_back("component");

        auto VaultFunc_UpdateComponent = engine->CreateFunction("Vault.UpdateComponent", 1, Vault_UpdateComponent);
        VaultFunc_UpdateComponent->argument_names.push_back("component");
    }

    // HYPERSCRIPT FUNCTIONS

    void HyperScriptManager::Scripts_OnStart() {
        for (auto &script : loaded_scripts) {
            auto &engine = script.second;
            std::cout << "Loading HyperScript: " << script.first << "\n";

            SetComponentStructures(engine);
            RegisterFunctions(engine);

            engine->LoadScript(script.first, script.first);
            engine->JustRunTheScript(engine->GetModule(script.first));
            engine->CreateVariable(HyperScript::GT_STRING, "GameObject.ID", ID);

            std::shared_ptr<HyperScript::Function> function_OnStart = engine->GetFunction("OnStart");
            function_OnStart->Call();
        }
    }

    void HyperScriptManager::Update() {
        for (auto &script : loaded_scripts) {
            auto &engine = script.second;

            std::shared_ptr<HyperScript::Function> function_OnUpdate = engine->GetFunction("OnUpdate");
            function_OnUpdate->Call();
            function_OnUpdate->arguments.push_back(HyperScript::Variable{HyperScript::GT_STRING, std::to_string(Runtime::instance->timestep), "content"});
            function_OnUpdate->arguments.back().name = function_OnUpdate->argument_names[function_OnUpdate->arguments.size() - 1];
        }
    }

    void HyperScriptManager::OnGUI() {
        DrawComponent<HyperScriptManager>(Scene::Main->EntityRegistry, 40, [&] {
            const bool pressed = ImGui::Button("Drag HyperScript (.hyper) File or Click");
            if (ImGui::BeginDragDropTarget()) {
                // clang-format off
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string script_path = (char *)payload->Data;
                    loaded_scripts[script_path] = std::make_shared<HyperScript::ScriptEngine>();
                }
                // clang-format on
            }

            if (pressed) {
                ImGuiFileDialog::Instance()->OpenDialog("HyperScript_LoadScript", ICON_FA_FLOPPY_DISK " Load HyperScript", ".hyper", "../assets");
            }

            if (ImGuiFileDialog::Instance()->Display("HyperScript_LoadScript")) {
                // action if OK
                // clang-format off
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string script_path = ImGuiFileDialog::Instance()->GetFilePathName();
                    loaded_scripts[script_path] = std::make_shared<HyperScript::ScriptEngine>();
                }
                // clang-format on

                ImGuiFileDialog::Instance()->Close();
            }

            for (auto &scripts : loaded_scripts) {
                ImGui::Text("%s", scripts.first.c_str());
            }
        });
    }

    void HyperScriptManager::Init() {
        transform = static_registry ? &Scene::StaticGameObjects_EntityRegistry.get<Transform>(entity) : &Scene::Main->EntityRegistry.get<Transform>(entity);
        transform->rotation.z = -1;
    }
} // namespace Engine::Components