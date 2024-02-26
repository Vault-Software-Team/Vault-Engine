#include "Box2D/Collision/Shapes/b2PolygonShape.h"
#include "Engine/Components/CSharpScriptComponent.hpp"
#include "mono/metadata/object-forward.h"
#include <Engine/Scene.hpp>
#include <iostream>
#include <Engine/GameObject.hpp>
#include <Engine/Runtime.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <memory>

namespace Engine {
    DLL_API std::shared_ptr<Scene> Scene::Main;
    DLL_API std::shared_ptr<Scene> Scene::EditorScene;
    DLL_API entt::registry Scene::StaticGameObjects_EntityRegistry;

    Components::Camera *Scene::EditorSceneCamera = nullptr;
    std::vector<std::shared_ptr<GameObject>> Scene::StaticGameObjects;

    void PhysisContactListener::BeginContact(b2Contact *contact) {
        const std::string &ID_A = *(std::string *)contact->GetFixtureA()->GetBody()->GetUserData();
        const std::string &ID_B = *(std::string *)contact->GetFixtureB()->GetBody()->GetUserData();
        auto &gameObjectA = GameObject::FindGameObjectByID(ID_A);
        auto &gameObjectB = GameObject::FindGameObjectByID(ID_B);

        if (gameObjectA->HasComponent<Components::CSharpScriptComponent>()) {
            auto &component = gameObjectA->GetComponent<Components::CSharpScriptComponent>();
            for (auto s : component.script_instances) {
                auto &script = s.second;

                MonoObject *exception = nullptr;
                void *p = mono_string_new(CSharp::instance->app_domain, gameObjectB->ID.c_str());
                MonoMethod *method = script->GetMethod("OnCollisionEnter2D", 1);
                if (method) {
                    mono_runtime_invoke(method, script->GetHandleTarget(), &p, &exception);

                    if (exception) {
                        MonoObject *exc = NULL;
                        MonoString *str = mono_object_to_string(exception, &exc);
                        if (exc) {
                            mono_print_unhandled_exception(exc);
                        } else {
                            std::cout << mono_string_to_utf8(str) << "\n";
                            // Log log(mono_string_to_utf8(str), LOG_ERROR);
                        }
                    }
                }
            }
        }

        if (gameObjectB->HasComponent<Components::CSharpScriptComponent>()) {
            auto &component = gameObjectB->GetComponent<Components::CSharpScriptComponent>();
            for (auto s : component.script_instances) {
                auto &script = s.second;

                MonoObject *exception = nullptr;
                void *p = mono_string_new(CSharp::instance->app_domain, gameObjectA->ID.c_str());
                MonoMethod *method = script->GetMethod("OnCollisionEnter2D", 1);
                if (method) {
                    mono_runtime_invoke(method, script->GetHandleTarget(), &p, &exception);

                    if (exception) {
                        MonoObject *exc = NULL;
                        MonoString *str = mono_object_to_string(exception, &exc);
                        if (exc) {
                            mono_print_unhandled_exception(exc);
                        } else {
                            std::cout << mono_string_to_utf8(str) << "\n";
                            // Log log(mono_string_to_utf8(str), LOG_ERROR);
                        }
                    }
                }
            }
        }

        std::cout << gameObjectA->name << " Collided with " << gameObjectB->name << "\n";
    }

    void PhysisContactListener::EndContact(b2Contact *contact) {
        const std::string &ID_A = *(std::string *)contact->GetFixtureA()->GetBody()->GetUserData();
        const std::string &ID_B = *(std::string *)contact->GetFixtureB()->GetBody()->GetUserData();
        auto &gameObjectA = GameObject::FindGameObjectByID(ID_A);
        auto &gameObjectB = GameObject::FindGameObjectByID(ID_B);

        if (gameObjectA->HasComponent<Components::CSharpScriptComponent>()) {
            auto &component = gameObjectA->GetComponent<Components::CSharpScriptComponent>();
            for (auto s : component.script_instances) {
                auto &script = s.second;

                MonoObject *exception = nullptr;
                void *p = mono_string_new(CSharp::instance->app_domain, gameObjectB->ID.c_str());
                MonoMethod *method = script->GetMethod("OnCollisionExit2D", 1);
                if (method) {
                    mono_runtime_invoke(method, script->GetHandleTarget(), &p, &exception);

                    if (exception) {
                        MonoObject *exc = NULL;
                        MonoString *str = mono_object_to_string(exception, &exc);
                        if (exc) {
                            mono_print_unhandled_exception(exc);
                        } else {
                            std::cout << mono_string_to_utf8(str) << "\n";
                            // Log log(mono_string_to_utf8(str), LOG_ERROR);
                        }
                    }
                }
            }
        }

        if (gameObjectB->HasComponent<Components::CSharpScriptComponent>()) {
            auto &component = gameObjectB->GetComponent<Components::CSharpScriptComponent>();
            for (auto s : component.script_instances) {
                auto &script = s.second;

                MonoObject *exception = nullptr;
                void *p = mono_string_new(CSharp::instance->app_domain, gameObjectA->ID.c_str());
                MonoMethod *method = script->GetMethod("OnCollisionExit2D", 1);
                if (method) {
                    mono_runtime_invoke(method, script->GetHandleTarget(), &p, &exception);

                    if (exception) {
                        MonoObject *exc = NULL;
                        MonoString *str = mono_object_to_string(exception, &exc);
                        if (exc) {
                            mono_print_unhandled_exception(exc);
                        } else {
                            std::cout << mono_string_to_utf8(str) << "\n";
                            // Log log(mono_string_to_utf8(str), LOG_ERROR);
                        }
                    }
                }
            }
        }
    }

    Scene::Scene(const std::string &scene_file) : scene_file_path(scene_file) {}

    std::shared_ptr<Scene> Scene::New(const std::string &scene_file) {
        return std::move(std::make_shared<Scene>(scene_file));
    }

    void Scene::SetMainScene(std::shared_ptr<Scene> &scene) {
        Main = scene;
    }

    void Scene::SetEditorScene(std::shared_ptr<Scene> &scene) {
        EditorScene = scene;
    }

    std::shared_ptr<GameObject> &Scene::MakeGameObject(const std::string &name, const std::string &tag) {
        GameObjects.push_back(std::make_shared<GameObject>(name, tag));
        return GameObjects.back();
    }

    Scene::~Scene() {
        Editor::GUI::selected_gameObject = nullptr;
        std::cout << "Deleting scene...\n";
        GameObjects.clear();
        main_camera_object = nullptr;
    }

    void Scene::SetMainCameraObject(const std::shared_ptr<GameObject> &camObject, bool static_registry) {
        main_camera_object = static_registry ? &camObject->GetComponent<Components::Camera>(StaticGameObjects_EntityRegistry) : &camObject->GetComponent<Components::Camera>();
    }

    void Scene::UpdateGameObjectComponents() {
        for (auto gameObject : GameObjects) {
            gameObject->UpdateComponents(*Runtime::default_shader);
        }
    }

    void Scene::UpdateStaticGameObjectComponents() {
        for (auto gameObject : StaticGameObjects) {
            gameObject->UpdateComponents(*Runtime::default_shader);
        }
    }

    void Scene::DeleteStaticGameObject(std::shared_ptr<GameObject> &gameObject) {
        for (int i = 0; i < StaticGameObjects.size(); i++) {
            if (StaticGameObjects[i]->entity == gameObject->entity) {
                StaticGameObjects.erase(StaticGameObjects.begin() + i);
                break;
            }
        }
    }

    void Scene::MakeSceneCamera() {
        StaticGameObjects.push_back(std::make_shared<GameObject>(StaticGameObjects_EntityRegistry, "VAULT_SceneCamera", "VAULT_SceneCamera"));
        auto &cam = StaticGameObjects.back();
        cam->AddComponent<Components::Camera>(StaticGameObjects_EntityRegistry);
        cam->GetComponent<Components::Transform>(StaticGameObjects_EntityRegistry).rotation.z = -1.0;

        cam->GetComponent<Components::Camera>(StaticGameObjects_EntityRegistry).fov = 45;
        cam->GetComponent<Components::Camera>(StaticGameObjects_EntityRegistry).near = 0.01f;
        cam->GetComponent<Components::Camera>(StaticGameObjects_EntityRegistry).far = 100.0f;

        cam->GetComponent<Components::Camera>(StaticGameObjects_EntityRegistry).width = 800;
        cam->GetComponent<Components::Camera>(StaticGameObjects_EntityRegistry).height = 600;
        cam->GetComponent<Components::Transform>(StaticGameObjects_EntityRegistry).position.y = 0.5f;
        cam->GetComponent<Components::Transform>(StaticGameObjects_EntityRegistry).position.z = 25.f;

        EditorSceneCamera = &cam->GetComponent<Components::Camera>(StaticGameObjects_EntityRegistry);
    }

    void Scene::SetSceneCameraAsMain() {
        main_camera_object = EditorSceneCamera;
    }

    std::shared_ptr<GameObject> &Scene::FindGameObjectByEntity(const entt::entity &entity) {
        for (auto &pGameObject : GameObjects) {
            if (pGameObject->entity == entity)
                return pGameObject;
        }
    }
    std::shared_ptr<GameObject> &Scene::FindGameObjectByID(const std::string &ID) {
        for (auto &pGameObject : GameObjects) {
            if (pGameObject->ID == ID)
                return pGameObject;
        }
    }

    std::shared_ptr<GameObject> &Scene::FindGameObjectByName(const std::string &name) {
        for (auto &pGameObject : GameObjects) {
            if (pGameObject->name == name)
                return pGameObject;
        }
    }

    template <typename T>
    static void CopyComponent(entt::registry &dest, entt::registry &src) {
        using namespace Components;
        auto src_view = src.view<T>();

        for (auto se : src_view) {
            auto &component = src.get<T>(se);
            auto dest_view = dest.view<Transform>();
            for (auto de : dest_view) {
                if (dest_view.get<Transform>(de).ID == src.get<Transform>(de).ID) {
                    dest.emplace_or_replace<T>(de, component);
                }
            }
        }
    }

    std::shared_ptr<Scene> Scene::Copy(std::shared_ptr<Scene> &other) {
        using namespace Components;
        std::shared_ptr<Scene> new_scene = Scene::New("");
        // Scene::SetEditorScene(other);
        // Scene::SetMainScene(new_scene);

        auto &source_reg = Scene::Main->EntityRegistry;
        auto &destination_reg = new_scene->EntityRegistry;

        auto view = source_reg.view<Transform>();
        for (auto e : view) {
            GameObject *gameObject = nullptr;
            for (auto pGameObject : Scene::Main->GameObjects) {
                if (pGameObject->entity == e) {
                    gameObject = pGameObject.get();
                    break;
                }
            }
            if (gameObject == nullptr)
                continue;

            // auto &new_gameObject = new_scene->MakeGameObject(gameObject->name, gameObject->tag);
            new_scene->GameObjects.push_back(std::make_shared<GameObject>(destination_reg, gameObject->name, gameObject->tag));
            auto &new_gameObject = new_scene->GameObjects.back();
            new_gameObject->name = gameObject->name;
            new_gameObject->tag = gameObject->tag;
            new_gameObject->ID = gameObject->ID;
            new_gameObject->parent = gameObject->parent;

            auto &transform = new_gameObject->GetComponent<Transform>(destination_reg);
            transform.ID = new_gameObject->ID;
        }

        CopyComponent<Transform>(destination_reg, source_reg);
        CopyComponent<Camera>(destination_reg, source_reg);
        CopyComponent<MeshRenderer>(destination_reg, source_reg);
        CopyComponent<AmbientLight>(destination_reg, source_reg);
        CopyComponent<PointLight>(destination_reg, source_reg);
        CopyComponent<DirectionalLight>(destination_reg, source_reg);
        CopyComponent<SpotLight>(destination_reg, source_reg);
        CopyComponent<Text3D>(destination_reg, source_reg);
        CopyComponent<BoxCollider2D>(destination_reg, source_reg);
        CopyComponent<Rigidbody2D>(destination_reg, source_reg);

        return new_scene;
    }

    DLL_API std::shared_ptr<Scene> Scene::CopyAndSetAsMain(std::shared_ptr<Scene> &other) {
        auto new_scene = Copy(other);
        Scene::SetMainScene(new_scene);
        Scene::Main->SetMainCameraObject(Scene::StaticGameObjects.back(), true);
        return new_scene;
    }
    void Scene::Setup2DPhysicsWorld() {
        Physics2DWorld.reset();
        Physics2DWorld = std::make_unique<b2World>((b2Vec2){0, -9.8});

        b2_contact_listener.reset();
        b2_contact_listener = std::make_unique<PhysisContactListener>();
        Physics2DWorld->SetContactListener(b2_contact_listener.get());

        auto view = EntityRegistry.view<Components::Rigidbody2D>();

        for (auto e : view) {
            auto &transform = EntityRegistry.get<Components::Transform>(e);
            auto &rigidbody = EntityRegistry.get<Components::Rigidbody2D>(e);

            b2BodyDef def;
            def.type = (b2BodyType)rigidbody.body_type;
            def.position.Set(transform.position.x, transform.position.y);
            def.angle = transform.rotation.z;

            b2Body *body = Physics2DWorld->CreateBody(&def);
            body->SetFixedRotation(rigidbody.fixed_rotation);
            body->SetGravityScale(rigidbody.gravity_scale);
            body->SetUserData(&rigidbody.ID);
            rigidbody.m_RuntimeBody = body;

            if (EntityRegistry.all_of<Components::BoxCollider2D>(e)) {
                auto &collider = EntityRegistry.get<Components::BoxCollider2D>(e);
                b2PolygonShape shape;
                shape.SetAsBox((((collider.size.x) / 2) - 0.02) / 2, (((collider.size.y) / 2) - 0.02) / 2);

                b2FixtureDef fixture_def;
                fixture_def.shape = &shape;
                fixture_def.density = collider.density;
                fixture_def.restitution = collider.restitution;
                fixture_def.friction = collider.friction;
                fixture_def.isSensor = collider.trigger;
                body->CreateFixture(&fixture_def);
            }
        }
    }

    void Scene::Step2DPhysicsWorld(const float ts) {
        const int32_t velocity_iter = 6;
        const int32_t pos_iter = 2;
        if (!Physics2DWorld)
            return;

        Physics2DWorld->Step(ts, velocity_iter, pos_iter);

        auto view = EntityRegistry.view<Components::Rigidbody2D>();
        for (auto e : view) {
            auto &transform = EntityRegistry.get<Components::Transform>(e);
            auto &rigidbody = EntityRegistry.get<Components::Rigidbody2D>(e);

            b2Body *body = (b2Body *)rigidbody.m_RuntimeBody;
            const b2Vec2 &pos = body->GetPosition();
            transform.position.x = pos.x;
            transform.position.y = pos.y;
            transform.rotation.z = body->GetAngle();
        }
    }

    void Scene::SetupHyperScript() {
        // auto view = EntityRegistry.view<Components::HyperScriptManager>();

        // for (auto e : view) {
        //     auto &manager = EntityRegistry.get<Components::HyperScriptManager>(e);
        //     manager.Scripts_OnStart();
        // }
    }

    void Scene::UpdateHyperScript() {
        // if (!Runtime::instance->isRunning) return;
        // auto view = EntityRegistry.view<Components::HyperScriptManager>();

        // for (auto e : view) {
        //     auto &manager = EntityRegistry.get<Components::HyperScriptManager>(e);
        //     manager.Update();
        // }
    }

    void Scene::SetupCSharp() {
        auto view = EntityRegistry.view<Components::CSharpScriptComponent>();

        for (auto e : view) {
            auto &manager = EntityRegistry.get<Components::CSharpScriptComponent>(e);
            manager.OnStart();
        }
    }

    void Scene::UpdateCSharp() {
        if (!Runtime::instance->isRunning) return;
        auto view = EntityRegistry.view<Components::CSharpScriptComponent>();

        for (auto e : view) {
            auto &manager = EntityRegistry.get<Components::CSharpScriptComponent>(e);
            manager.Update();
        }
    }

    void Scene::OnRuntimeStart() {
        Setup2DPhysicsWorld();
        SetupHyperScript();
        SetupCSharp();
    }

    void Scene::OnRuntimeStop() {
        Physics2DWorld.reset();
    }

    void Scene::OnRuntimeUpdate(const float ts) {
        Step2DPhysicsWorld(ts);
        UpdateHyperScript();
        UpdateCSharp();
    }
} // namespace Engine