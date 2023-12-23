#pragma once
#include <string>
#include <entt/entt.hpp>
#include <Engine/Scene.hpp>

namespace Engine {
    class DLL_API GameObject {
    public:
        static std::shared_ptr<GameObject> &New(const std::string &name, const std::string &tag = "Default");

        std::string ID, name, tag;
        entt::entity entity;
        std::list<std::shared_ptr<GameObject>> children;

        GameObject(const std::string &name, const std::string &tag = "Default");

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
    };
} // namespace Engine