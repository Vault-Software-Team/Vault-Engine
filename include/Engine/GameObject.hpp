#pragma once
#include "Engine/Components/Transform.hpp"
#include <dllapi.hpp>
#include <string>
#include <entt/entt.hpp>
#include <Engine/Scene.hpp>
#include <iostream>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
#else
    #include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <Renderer/Shader.hpp>

namespace Engine {
    class DLL_API GameObject : public std::enable_shared_from_this<GameObject> {
    private:
        void GUI_ContextMenu();
        std::string icon;
        bool another_registry = false;
        entt::registry *other_registry = nullptr;

    public:
        Components::Transform *transform;
        static DLL_API std::vector<std::shared_ptr<GameObject>> scheduled_deletions;

        static DLL_API std::shared_ptr<GameObject> &FindGameObjectByEntity(const entt::entity &entity);
        static DLL_API std::shared_ptr<GameObject> &FindGameObjectByID(const std::string &id);
        static DLL_API std::shared_ptr<GameObject> &FindGameObjectByName(const std::string &name);
        static DLL_API std::shared_ptr<GameObject> &FindGameObjectByTag(const std::string &tag);

        static DLL_API std::shared_ptr<GameObject> &New(const std::string &name, const std::string &tag = "Default");
        static DLL_API std::shared_ptr<GameObject> &NewStatic(const std::string &name, const std::string &tag = "Default");

        std::string ID, name, tag;
        entt::entity entity;
        std::string parent = "NO_PARENT";

        GameObject(const std::string &name, const std::string &tag = "Default");
        GameObject(entt::registry &registry, const std::string &name, const std::string &tag = "Default");
        ~GameObject();

        void UpdateComponents(VaultRenderer::Shader &shader);
        void UpdateRendering(VaultRenderer::Shader &shader, bool update_children = false);
        void SetParent(std::shared_ptr<GameObject> &parent);
        std::shared_ptr<GameObject> &AddChild(const std::string &name, const std::string &tag = "Default");
        std::shared_ptr<GameObject> &AddChild(std::shared_ptr<GameObject> &child);

        template <typename T, typename... Args>
        T &AddComponent(Args &&...args) {
            if (!HasComponent<T>() && Scene::Main->EntityRegistry.valid(entity)) {
                T &component = Scene::Main->EntityRegistry.emplace<T>(
                    entity, std::forward<Args>(args)...);

                auto &comp = GetComponent<T>();
                comp.entity = entity;
                comp.ID = ID;
                comp.Init();

                return comp;
            }
        }

        template <typename T>
        T &GetComponent() {
            if (HasComponent<T>()) {
                return Scene::Main->EntityRegistry.get<T>(entity);
            } else {
                T comp;
                return comp;
            }
        }

        template <typename T>
        bool HasComponent() {
            // if (Scene::Main->EntityRegistry.valid(entity)) {
            return Scene::Main->EntityRegistry.all_of<T>(entity);
            // }

            return false;
        }

        template <typename T>
        void RemoveComponent() {
            Scene::Main->EntityRegistry.remove<T>(entity);
        }

        template <typename T, typename... Args>
        T &AddComponent(entt::registry &registry, Args &&...args) {
            if (!HasComponent<T>(registry) && registry.valid(entity)) {
                T &component = registry.emplace<T>(
                    entity, std::forward<Args>(args)...);

                auto &comp = GetComponent<T>(registry);
                comp.entity = entity;
                comp.static_registry = true;
                comp.ID = ID;
                comp.Init();

                return comp;
            }
        }

        template <typename T>
        T &GetComponent(entt::registry &registry) {
            if (HasComponent<T>(registry)) {
                return registry.get<T>(entity);
            } else {
                T comp;
                return comp;
            }
        }

        template <typename T>
        bool HasComponent(entt::registry &registry) {
            // if (registry.valid(entity)) {
            return registry.all_of<T>(entity);
            // }

            return false;
        }

        template <typename T>
        void RemoveComponent(entt::registry &registry) {
            registry.remove<T>(entity);
        }

        void UNSAFE_DeleteGameObject();

        void GUI();
    };
} // namespace Engine