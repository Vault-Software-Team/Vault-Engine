#include "Engine/Components/BoxCollider2D.hpp"
#include "Engine/Components/CSharpScriptComponent.hpp"
#include "Engine/Components/Rigidbody2D.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Mono/HelperFunctions.hpp"
#include "Engine/Mono/GameObject/Functions.hpp"
#include "Engine/Runtime.hpp"
#include "Engine/Scene.hpp"
#include "Engine/SceneSerialization.hpp"
#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine::CSharpInternalFunctions {
    MonoString *GameObject_GetName(MonoString *ID) {
        auto gameObject = GameObject::FindGameObjectByID(CSharpHelper::MonoStrToString(ID));
        if (!gameObject) {
            return CSharpHelper::StrToMonoString("GAMEOBJECT_NOT_FOUND");
        }
        return CSharpHelper::StrToMonoString(gameObject->name);
    }
    MonoString *GameObject_GetTag(MonoString *ID) {
        auto gameObject = GameObject::FindGameObjectByID(CSharpHelper::MonoStrToString(ID));
        if (!gameObject) {
            return CSharpHelper::StrToMonoString("GAMEOBJECT_NOT_FOUND");
        }
        return CSharpHelper::StrToMonoString(gameObject->tag);
    }

    MonoString *GameObject_GetIDByName(MonoString *name) {
        auto gameObject = GameObject::FindGameObjectByName(CSharpHelper::MonoStrToString(name));
        if (!gameObject) {
            return CSharpHelper::StrToMonoString("GAMEOBJECT_NOT_FOUND");
        }
        return CSharpHelper::StrToMonoString(gameObject->ID);
    }
    MonoString *GameObject_GetIDByTag(MonoString *tag) {
        auto gameObject = GameObject::FindGameObjectByTag(CSharpHelper::MonoStrToString(tag));
        if (!gameObject) {
            return CSharpHelper::StrToMonoString("GAMEOBJECT_NOT_FOUND");
        }
        return CSharpHelper::StrToMonoString(gameObject->ID);
    }

    void Scene_LoadScene(MonoString *path) {
        Serializer::scheduled_scene_path = CSharpHelper::MonoStrToString(path);
    }

    MonoString *GameObject_InstantiatePrefab(MonoString *ms_path, MonoString *ms_name, MonoString *ms_tag, MonoString *ms_id) {
        const std::string path = CSharpHelper::MonoStrToString(ms_path);
        const std::string name = CSharpHelper::MonoStrToString(ms_name);
        const std::string tag = CSharpHelper::MonoStrToString(ms_tag);
        const std::string id = CSharpHelper::MonoStrToString(ms_id);

        auto gameObject = Serializer::LoadPrefab(path);
        gameObject->ID = id == "DEFAULT" ? gameObject->ID : id;
        gameObject->name = name == "__VAULT_DEFAULT_NAME__" ? gameObject->name : name;
        gameObject->tag = tag == "__VAULT_DEFAULT_TAG__" ? gameObject->tag : tag;

        if (!Runtime::instance->isRunning) return CSharpHelper::StrToMonoString(gameObject->ID);

        if (gameObject->HasComponent<Components::CSharpScriptComponent>()) {
            auto &manager = gameObject->GetComponent<Components::CSharpScriptComponent>();
            manager.OnStart();
        }

        if (gameObject->HasComponent<Components::Rigidbody2D>()) {
            auto &transform = gameObject->GetComponent<Components::Transform>();
            auto &rigidbody = gameObject->GetComponent<Components::Rigidbody2D>();

            b2BodyDef def;
            def.type = (b2BodyType)rigidbody.body_type;
            def.position.Set(transform.position.x, transform.position.y);
            def.angle = transform.rotation.z;

            b2Body *body = Scene::Main->Physics2DWorld->CreateBody(&def);
            body->SetFixedRotation(rigidbody.fixed_rotation);
            body->SetGravityScale(rigidbody.gravity_scale);
            body->SetUserData(&rigidbody.ID);
            rigidbody.m_RuntimeBody = body;

            if (gameObject->HasComponent<Components::BoxCollider2D>()) {
                auto &collider = gameObject->GetComponent<Components::BoxCollider2D>();
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

        return CSharpHelper::StrToMonoString(gameObject->ID);
    }
    // have to do this fucked up traumatizing shenanigan and fuckery cause idk why Vector3 (C#) doesn't convert to glm::vec3
    MonoString *GameObject_InstantiatePrefabWithProps(MonoString *ms_path, float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz, MonoString *ms_name, MonoString *ms_tag, MonoString *ms_id) {
        const std::string path = CSharpHelper::MonoStrToString(ms_path);
        const std::string name = CSharpHelper::MonoStrToString(ms_name);
        const std::string tag = CSharpHelper::MonoStrToString(ms_tag);
        const std::string id = CSharpHelper::MonoStrToString(ms_id);

        auto gameObject = Serializer::LoadPrefab(path);
        gameObject->ID = id == "DEFAULT" ? gameObject->ID : id;
        gameObject->name = name == "__VAULT_DEFAULT_NAME__" ? gameObject->name : name;
        gameObject->tag = tag == "__VAULT_DEFAULT_TAG__" ? gameObject->tag : tag;
        auto &transform = gameObject->GetComponent<Components::Transform>();
        transform.position = glm::vec3(px, py, pz);
        transform.rotation = glm::vec3(rx, ry, rz);
        transform.scale = glm::vec3(sx, sy, sz);

        if (!Runtime::instance->isRunning) return CSharpHelper::StrToMonoString(gameObject->ID);

        if (gameObject->HasComponent<Components::CSharpScriptComponent>()) {
            auto &manager = gameObject->GetComponent<Components::CSharpScriptComponent>();
            manager.OnStart();
        }

        if (gameObject->HasComponent<Components::Rigidbody2D>()) {
            auto &rigidbody = gameObject->GetComponent<Components::Rigidbody2D>();

            b2BodyDef def;
            def.type = (b2BodyType)rigidbody.body_type;
            def.position.Set(transform.position.x, transform.position.y);
            def.angle = transform.rotation.z;

            b2Body *body = Scene::Main->Physics2DWorld->CreateBody(&def);
            body->SetFixedRotation(rigidbody.fixed_rotation);
            body->SetGravityScale(rigidbody.gravity_scale);
            body->SetUserData(&rigidbody.ID);
            rigidbody.m_RuntimeBody = body;

            if (gameObject->HasComponent<Components::BoxCollider2D>()) {
                auto &collider = gameObject->GetComponent<Components::BoxCollider2D>();
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

        return CSharpHelper::StrToMonoString(gameObject->ID);
    }
} // namespace Engine::CSharpInternalFunctions