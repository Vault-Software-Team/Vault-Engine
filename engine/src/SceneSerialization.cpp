#include "Engine/Components/BoneManipulator.hpp"
#include "Engine/Components/CSharpScriptComponent.hpp"
#include "Engine/Components/Collider3D.hpp"
#include "Engine/Components/ModelAnimator.hpp"
#include "Engine/Components/Rigidbody3D.hpp"
#include "Engine/Components/SpriteRenderer.hpp"
#include "Engine/PostProcessing.hpp"
#include "Engine/Runtime.hpp"
#include "Renderer/Window.hpp"
#include "glm/fwd.hpp"
#include "yaml-cpp/emittermanip.h"
#include <Engine/SceneSerialization.hpp>
#include <Engine/GameObject.hpp>
#include <filesystem>
#include <fstream>
#include <Engine/Components/IncludeComponents.hpp>
#include <iostream>
#include <Engine/Model.hpp>
#include <sstream>
#include <string>
#include <unordered_map>
#include <uuid.hpp>
#include <experimental/filesystem>

namespace fs = std::filesystem;

namespace YAML {
    template <>
    struct convert<glm::vec2> {
        static Node encode(const glm::vec2 &rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node &node, glm::vec2 &rhs) {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();

            return true;
        }
    };

    template <>
    struct convert<glm::vec3> {
        static Node encode(const glm::vec3 &rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node &node, glm::vec3 &rhs) {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();

            return true;
        }
    };

    template <>
    struct convert<glm::vec4> {
        static Node encode(const glm::vec4 &rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node &node, glm::vec4 &rhs) {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();

            return true;
        }
    };
} // namespace YAML

namespace Engine {
    yaml::Emitter &operator<<(yaml::Emitter &out, const glm::vec2 &vec) {
        out << yaml::Flow;
        out << yaml::BeginSeq << vec.x << vec.y << yaml::EndSeq;
        return out;
    }

    yaml::Emitter &operator<<(yaml::Emitter &out, const glm::vec3 &vec) {
        out << yaml::Flow;
        out << yaml::BeginSeq << vec.x << vec.y << vec.z << yaml::EndSeq;
        return out;
    }

    yaml::Emitter &operator<<(yaml::Emitter &out, const glm::vec4 &vec) {
        out << yaml::Flow;
        out << yaml::BeginSeq << vec.x << vec.y << vec.z << vec.w << yaml::EndSeq;
        return out;
    }

    void Serializer::Serialize(std::shared_ptr<Scene> &scene, const std::string &path) {
        yaml::Emitter emitter;
        emitter << yaml::BeginMap;
        emitter << yaml::Key << "Scene" << yaml::Value << "Untitled";
        emitter << yaml::Key << "GameObjects" << yaml::Value << yaml::BeginSeq;

        for (auto &gameObject : scene->GameObjects) {
            EntityToYAML(emitter, gameObject);
        }

        emitter << yaml::EndSeq;
        emitter << yaml::EndMap;

        std::ofstream file(path);
        file << emitter.c_str();
    }
    void Serializer::EntityToYAML(yaml::Emitter &emitter, std::shared_ptr<GameObject> &gameObject) {
        using namespace Components;
        emitter << yaml::BeginMap;

        emitter << yaml::Key << "GameObject" << yaml::BeginMap;
        emitter << yaml::Key << "ID" << yaml::Value << gameObject->ID;
        emitter << yaml::Key << "name" << yaml::Value << gameObject->name;
        emitter << yaml::Key << "tag" << yaml::Value << gameObject->tag;
        emitter << yaml::Key << "parent" << yaml::Value << gameObject->parent;

        // Components
        if (gameObject->HasComponent<Transform>()) {
            emitter << yaml::Key << "Transform";
            emitter << yaml::BeginMap;

            auto &transform = gameObject->GetComponent<Transform>();
            emitter << yaml::Key << "position" << yaml::Value << transform.position;
            emitter << yaml::Key << "rotation" << yaml::Value << transform.rotation;
            emitter << yaml::Key << "scale" << yaml::Value << transform.scale;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<AmbientLight>()) {
            emitter << yaml::Key << "AmbientLight";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<AmbientLight>();
            emitter << yaml::Key << "amount" << yaml::Value << component.amount;
            emitter << yaml::Key << "color" << yaml::Value << component.color;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<PointLight>()) {
            emitter << yaml::Key << "PointLight";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<PointLight>();
            emitter << yaml::Key << "intensity" << yaml::Value << component.intensity;
            emitter << yaml::Key << "color" << yaml::Value << component.color;
            emitter << yaml::Key << "enable_shadow_mapping" << yaml::Value << component.enable_shadow_mapping;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<DirectionalLight>()) {
            emitter << yaml::Key << "DirectionalLight";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<DirectionalLight>();
            emitter << yaml::Key << "intensity" << yaml::Value << component.intensity;
            emitter << yaml::Key << "color" << yaml::Value << component.color;
            emitter << yaml::Key << "enable_shadow_mapping" << yaml::Value << component.enable_shadow_mapping;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<SpotLight>()) {
            emitter << yaml::Key << "SpotLight";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<SpotLight>();
            emitter << yaml::Key << "intensity" << yaml::Value << component.intensity;
            emitter << yaml::Key << "color" << yaml::Value << component.color;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<Text3D>()) {
            emitter << yaml::Key << "Text3D";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<Text3D>();
            emitter << yaml::Key << "text" << yaml::Value << component.text;
            emitter << yaml::Key << "font_path" << yaml::Value << component.font->font_path;
            emitter << yaml::Key << "color" << yaml::Value << component.color;
            emitter << yaml::Key << "emissionColor" << yaml::Value << component.emissionColor;
            emitter << yaml::Key << "scale" << yaml::Value << component.scale;
            emitter << yaml::Key << "y_offset" << yaml::Value << component.y_offset;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<MeshRenderer>()) {
            emitter << yaml::Key << "MeshRenderer";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<MeshRenderer>();
            emitter << yaml::Key << "mesh_type" << yaml::Value << component.mesh_type;
            emitter << yaml::Key << "mesh_index" << yaml::Value << component.mesh_index;
            emitter << yaml::Key << "mesh_path" << yaml::Value << component.mesh_path;
            emitter << yaml::Key << "material_path" << yaml::Value << component.material_path;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<ModelRenderer>()) {
            emitter << yaml::Key << "ModelRenderer";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<ModelRenderer>();
            emitter << yaml::Key << "model_path" << yaml::Value << (component.model == nullptr ? "nullptr" : component.model->path);
            emitter << yaml::Key << "time_scale" << yaml::Value << component.time_scale;
            emitter << yaml::Key << "play_animation" << yaml::Value << component.play_animation;
            emitter << yaml::Key << "animation_path" << yaml::Value << component.animation_path;

            emitter << yaml::Key << "mesh_materials";
            emitter << yaml::BeginMap;
            for (auto &mesh : component.model->meshes) {
                emitter << yaml::Key << mesh.name << yaml::Value << mesh.material.filePath;
            }
            emitter << yaml::EndMap;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<ModelAnimator>()) {
            emitter << yaml::Key << "ModelAnimator";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<ModelAnimator>();
            emitter << yaml::Key << "model_path" << yaml::Value << (component.model == nullptr ? "nullptr" : component.model->path);
            emitter << yaml::Key << "time_scale" << yaml::Value << component.time_scale;
            emitter << yaml::Key << "play_animation" << yaml::Value << component.play_animation;
            emitter << yaml::Key << "current_animation" << yaml::Value << component.current_animation;

            // emitter << yaml::Key << "animation_path" << yaml::Value << component.animation_path;

            emitter << yaml::Key << "animations";
            emitter << yaml::BeginMap;
            for (auto &mesh : component.animations) {
                emitter << yaml::Key << mesh.first << yaml::Value << mesh.second->animation_path;
            }
            emitter << yaml::EndMap;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<BoneManipulator>()) {
            emitter << yaml::Key << "BoneManipulator";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<BoneManipulator>();
            emitter << yaml::Key << "nodeName" << yaml::Value << component.nodeName;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<SpriteRenderer>()) {
            emitter << yaml::Key << "SpriteRenderer";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<SpriteRenderer>();
            emitter << yaml::Key << "color" << yaml::Value << component.mesh->material.color;
            emitter << yaml::Key << "emissionColor" << yaml::Value << component.mesh->material.emissionColor;
            emitter << yaml::Key << "texture" << yaml::Value << (component.mesh->material.diffuse == nullptr ? "nullptr" : component.mesh->material.diffuse->texture_data->texture_filepath);

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<SpritesheetRenderer>()) {
            emitter << yaml::Key << "SpritesheetRenderer";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<SpritesheetRenderer>();
            emitter << yaml::Key << "color" << yaml::Value << component.mesh->material.color;
            emitter << yaml::Key << "texture" << yaml::Value << (component.mesh->material.diffuse == nullptr ? "nullptr" : component.mesh->material.diffuse->texture_data->texture_filepath);
            emitter << yaml::Key << "spritesheet_size" << yaml::Value << component.spritesheetSize;
            emitter << yaml::Key << "sprite_size" << yaml::Value << component.spriteSize;
            emitter << yaml::Key << "sprite_offset" << yaml::Value << component.spriteOffset;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<Camera>()) {
            emitter << yaml::Key << "Camera";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<Camera>();
            emitter << yaml::Key << "fov" << yaml::Value << component.fov;
            emitter << yaml::Key << "near" << yaml::Value << component.near;
            emitter << yaml::Key << "far" << yaml::Value << component.far;
            emitter << yaml::Key << "main_camera" << yaml::Value << component.main_camera;
            emitter << yaml::Key << "is2D" << yaml::Value << component.is2D;
            emitter << yaml::Key << "depth_camera" << yaml::Value << component.depth_camera;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<Rigidbody2D>()) {
            emitter << yaml::Key << "Rigidbody2D";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<Rigidbody2D>();
            emitter << yaml::Key << "body_type" << yaml::Value << (int)component.body_type;
            emitter << yaml::Key << "gravity_scale" << yaml::Value << component.gravity_scale;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<Rigidbody3D>()) {
            emitter << yaml::Key << "Rigidbody3D";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<Rigidbody3D>();
            emitter << yaml::Key << "mass" << yaml::Value << component.mass;
            emitter << yaml::Key << "restitution" << yaml::Value << component.restitution;
            emitter << yaml::Key << "friction" << yaml::Value << component.friction;
            emitter << yaml::Key << "trigger" << yaml::Value << component.trigger;
            emitter << yaml::Key << "fixedRotation" << yaml::Value << component.fixedRotation;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<BoxCollider3D>()) {
            emitter << yaml::Key << "BoxCollider3D";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<BoxCollider3D>();

            emitter << yaml::Key << "size" << yaml::Value << component.size;
            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<MeshCollider3D>()) {
            emitter << yaml::Key << "MeshCollider3D";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<MeshCollider3D>();

            emitter << yaml::Key << "size" << yaml::Value << component.size;
            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<BoxCollider2D>()) {
            emitter << yaml::Key << "BoxCollider2D";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<BoxCollider2D>();
            emitter << yaml::Key << "density" << yaml::Value << component.density;
            emitter << yaml::Key << "friction" << yaml::Value << component.friction;
            emitter << yaml::Key << "restitution" << yaml::Value << component.restitution;
            emitter << yaml::Key << "trigger" << yaml::Value << component.trigger;
            emitter << yaml::Key << "offset" << yaml::Value << component.offset;
            emitter << yaml::Key << "size" << yaml::Value << component.size;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<CSharpScriptComponent>()) {
            emitter << yaml::Key << "CSharpScriptComponent";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<CSharpScriptComponent>();
            emitter << yaml::Key << "selected_scripts";
            emitter << yaml::BeginMap;

            for (auto s : component.selected_scripts) {
                emitter << yaml::Key << s.first << yaml::Value << (s.second.first == "" ? std::string("THIS_IS_EMPTY_FUCK_YOU_LOL") : "") + "." + s.second.second;
            }

            emitter << yaml::EndMap;

            emitter << yaml::EndMap;
        }

        emitter << YAML::EndMap;

        emitter << yaml::EndMap;
    }
    std::string Serializer::SerializeRuntime() {
        yaml::Emitter emitter;
        emitter << yaml::BeginMap;
        emitter << yaml::Key << "Scene" << yaml::Value << "Untitled";
        emitter << yaml::Key << "GameObjects" << yaml::Value << yaml::BeginSeq;

        for (auto &gameObject : Scene::Main->GameObjects) {
            EntityToYAML(emitter, gameObject);
        }

        emitter << yaml::EndSeq;
        emitter << yaml::EndMap;

        return emitter.c_str();
    }

    void Serializer::YAMLToEntity(yaml::Node &data, std::shared_ptr<GameObject> &gameObject) {
        using namespace Components;

        if (data["Transform"]) {
            auto &component = gameObject->GetComponent<Transform>();
            component.position = data["Transform"]["position"].as<glm::vec3>();
            component.rotation = data["Transform"]["rotation"].as<glm::vec3>();
            component.scale = data["Transform"]["scale"].as<glm::vec3>();
        }

        if (data["AmbientLight"]) {
            gameObject->AddComponent<AmbientLight>();
            auto &component = gameObject->GetComponent<AmbientLight>();
            component.amount = data["AmbientLight"]["amount"].as<float>();
            component.color = data["AmbientLight"]["color"].as<glm::vec3>();
        }

        if (data["PointLight"]) {
            gameObject->AddComponent<PointLight>();
            auto &component = gameObject->GetComponent<PointLight>();
            component.intensity = data["PointLight"]["intensity"].as<float>();
            component.color = data["PointLight"]["color"].as<glm::vec3>();
            component.enable_shadow_mapping = data["PointLight"]["enable_shadow_mapping"].as<bool>();
        }

        if (data["DirectionalLight"]) {
            gameObject->AddComponent<DirectionalLight>();
            auto &component = gameObject->GetComponent<DirectionalLight>();
            component.intensity = data["DirectionalLight"]["intensity"].as<float>();
            component.color = data["DirectionalLight"]["color"].as<glm::vec3>();
            component.enable_shadow_mapping = data["DirectionalLight"]["enable_shadow_mapping"].as<bool>();
        }

        if (data["SpotLight"]) {
            gameObject->AddComponent<SpotLight>();
            auto &component = gameObject->GetComponent<SpotLight>();
            component.intensity = data["SpotLight"]["intensity"].as<float>();
            component.color = data["SpotLight"]["color"].as<glm::vec3>();
        }

        if (data["Text3D"]) {
            gameObject->AddComponent<Text3D>();
            auto &component = gameObject->GetComponent<Text3D>();
            component.text = data["Text3D"]["text"].as<std::string>();
            component.color = data["Text3D"]["color"].as<glm::vec3>();
            component.scale = data["Text3D"]["scale"].as<float>();
            component.y_offset = data["Text3D"]["y_offset"].as<float>();
            component.ChangeFont(data["Text3D"]["font_path"].as<std::string>());

            if (data["Text3D"]["emissionColor"]) {
                component.emissionColor = data["Text3D"]["emissionColor"].as<glm::vec3>();
            }
        }

        if (data["MeshRenderer"]) {
            gameObject->AddComponent<MeshRenderer>();
            auto &component = gameObject->GetComponent<MeshRenderer>();
            if (data["MeshRenderer"]["material_path"]) {
                component.material_path = data["MeshRenderer"]["material_path"].as<std::string>();
            }
            if (data["MeshRenderer"]["mesh_path"]) {
                component.mesh_path = data["MeshRenderer"]["mesh_path"].as<std::string>();
            }
            if (data["MeshRenderer"]["mesh_index"]) {
                component.mesh_index = data["MeshRenderer"]["mesh_index"].as<int>();
            }

            component.SetMeshType((MeshType)data["MeshRenderer"]["mesh_type"].as<int>());
            if (component.mesh_type == Components::MESH_CUSTOM_MODEL && component.mesh_index > -1) {
                component.model = std::make_shared<ModelMesh>(component.mesh_path);

                component.mesh.reset();
                component.mesh = std::make_shared<VaultRenderer::Mesh>(component.model->meshes[component.mesh_index].vertices, component.model->meshes[component.mesh_index].indices);
            }
        }

        if (data["ModelRenderer"]) {
            gameObject->AddComponent<ModelRenderer>();
            auto &component = gameObject->GetComponent<ModelRenderer>();
            if (data["ModelRenderer"]["model_path"]) {
                component.model.reset();
                component.model = std::make_unique<ModelMesh>(data["ModelRenderer"]["model_path"].as<std::string>());
            }
            if (data["ModelRenderer"]["time_scale"]) {
                component.time_scale = data["ModelRenderer"]["time_scale"].as<float>();
            }
            if (data["ModelRenderer"]["play_animation"]) {
                component.play_animation = data["ModelRenderer"]["play_animation"].as<bool>();
            }

            if (data["ModelRenderer"]["animation_path"]) {
                const auto path = data["ModelRenderer"]["animation_path"].as<std::string>();
                if (path != "") {
                    component.animation_path = path;
                    component.SetAnimation(component.animation_path);
                }
            }

            if (data["ModelRenderer"]["mesh_materials"] && component.model) {
                std::unordered_map<std::string, std::string> mat_map;
                for (auto mesh_mat : data["ModelRenderer"]["mesh_materials"]) {
                    const std::string &mesh_name = mesh_mat.first.as<std::string>();
                    const std::string &material_path = mesh_mat.second.as<std::string>();
                    mat_map[mesh_name] = material_path;
                }

                for (auto &mesh : component.model->meshes) {
                    const auto &mesh_data = mat_map.find(mesh.name);
                    if (mesh_data == mat_map.end()) continue;
                    mesh.material.filePath = mesh_data->second;
                    DeserializeMaterial(mesh_data->second, mesh.material);
                }
            }
        }

        if (data["ModelAnimator"]) {
            gameObject->AddComponent<ModelAnimator>();
            auto &component = gameObject->GetComponent<ModelAnimator>();
            if (data["ModelAnimator"]["model_path"]) {
                component.model.reset();
                component.model = std::make_unique<ModelMesh>(data["ModelAnimator"]["model_path"].as<std::string>());
            }
            if (data["ModelAnimator"]["time_scale"]) {
                component.time_scale = data["ModelAnimator"]["time_scale"].as<float>();
            }
            if (data["ModelAnimator"]["play_animation"]) {
                component.play_animation = data["ModelAnimator"]["play_animation"].as<bool>();
            }

            if (data["ModelAnimator"]["current_animation"]) {
                component.current_animation = data["ModelAnimator"]["current_animation"].as<std::string>();
            }

            if (data["ModelRenderer"]["animations"]) {
                for (auto anim : data["ModelRenderer"]["animations"]) {
                    const std::string &name = anim.first.as<std::string>();
                    const std::string &path = anim.second.as<std::string>();
                    component.SetAnimation(name, path);
                }
            }

            // if (data["ModelAnimator"]["animation_path"]) {
            //     const auto path = data["ModelAnimator"]["animation_path"].as<std::string>();
            //     if (path != "") {
            //         component.animation_path = path;
            //         component.SetAnimation(component.animation_path);
            //     }
            // }
        }

        if (data["BoneManipulator"]) {
            gameObject->AddComponent<BoneManipulator>();
            auto &component = gameObject->GetComponent<BoneManipulator>();
            if (data["BoneManipulator"]["nodeName"]) {
                component.nodeName = data["BoneManipulator"]["nodeName"].as<std::string>();
            }
        }

        if (data["SpriteRenderer"]) {
            gameObject->AddComponent<SpriteRenderer>();
            auto &component = gameObject->GetComponent<SpriteRenderer>();
            if (data["SpriteRenderer"]["texture"] && data["SpriteRenderer"]["texture"].as<std::string>() != "nullptr") {
                component.mesh->material.SetDiffuse(data["SpriteRenderer"]["texture"].as<std::string>());
            }

            if (data["SpriteRenderer"]["color"]) {
                component.mesh->material.color = data["SpriteRenderer"]["color"].as<glm::vec4>();
            }

            if (data["SpriteRenderer"]["emissionColor"]) {
                component.mesh->material.emissionColor = data["SpriteRenderer"]["emissionColor"].as<glm::vec3>();
            }
        }

        if (data["SpritesheetRenderer"]) {
            gameObject->AddComponent<SpritesheetRenderer>();
            auto &component = gameObject->GetComponent<SpritesheetRenderer>();
            if (data["SpriteRenderer"]["texture"] && data["SpriteRenderer"]["texture"].as<std::string>() != "nullptr") {
                component.mesh->material.SetDiffuse(data["SpriteRenderer"]["texture"].as<std::string>());
            }

            if (data["SpriteRenderer"]["color"]) {
                component.mesh->material.color = data["SpriteRenderer"]["color"].as<glm::vec4>();
            }

            if (data["SpriteRenderer"]["spritesheet_size"]) {
                component.spritesheetSize = data["SpriteRenderer"]["spritesheet_size"].as<glm::vec2>();
            }

            if (data["SpriteRenderer"]["sprite_size"]) {
                component.spriteSize = data["SpriteRenderer"]["sprite_size"].as<glm::vec2>();
            }

            if (data["SpriteRenderer"]["sprite_offset"]) {
                component.spriteOffset = data["SpriteRenderer"]["sprite_offset"].as<glm::vec2>();
            }
        }

        if (data["Camera"]) {
            gameObject->AddComponent<Camera>();
            auto &component = gameObject->GetComponent<Camera>();
            component.fov = data["Camera"]["fov"].as<float>();
            component.near = data["Camera"]["near"].as<float>();
            component.far = data["Camera"]["far"].as<float>();
            component.main_camera = data["Camera"]["main_camera"].as<bool>();
            if (data["Camera"]["is2D"])
                component.is2D = data["Camera"]["is2D"].as<bool>();
            if (data["Camera"]["depth_camera"])
                component.depth_camera = data["Camera"]["depth_camera"].as<bool>();
        }

        if (data["Rigidbody2D"]) {
            gameObject->AddComponent<Rigidbody2D>();
            auto &component = gameObject->GetComponent<Rigidbody2D>();
            component.body_type = (Rigidbody2D::BodyType)data["Rigidbody2D"]["body_type"].as<int>();
            component.gravity_scale = data["Rigidbody2D"]["gravity_scale"].as<float>();
        }

        if (data["Rigidbody3D"]) {
            gameObject->AddComponent<Rigidbody3D>();
            auto &component = gameObject->GetComponent<Rigidbody3D>();
            component.mass = data["Rigidbody3D"]["mass"].as<float>();
            component.restitution = data["Rigidbody3D"]["restitution"].as<float>();
            component.friction = data["Rigidbody3D"]["friction"].as<float>();
            component.trigger = data["Rigidbody3D"]["trigger"].as<bool>();
            component.fixedRotation = data["Rigidbody3D"]["fixedRotation"].as<bool>();
        }

        if (data["BoxCollider3D"]) {
            gameObject->AddComponent<BoxCollider3D>();
            auto &component = gameObject->GetComponent<BoxCollider3D>();
            component.size = data["BoxCollider3D"]["size"].as<glm::vec3>();
        }

        if (data["MeshCollider3D"]) {
            gameObject->AddComponent<MeshCollider3D>();
            auto &component = gameObject->GetComponent<MeshCollider3D>();
            component.size = data["MeshCollider3D"]["size"].as<glm::vec3>();
        }

        if (data["BoxCollider2D"]) {
            gameObject->AddComponent<BoxCollider2D>();
            auto &component = gameObject->GetComponent<BoxCollider2D>();
            component.density = data["BoxCollider2D"]["density"].as<float>();
            component.friction = data["BoxCollider2D"]["friction"].as<float>();
            component.restitution = data["BoxCollider2D"]["restitution"].as<float>();
            component.offset = data["BoxCollider2D"]["offset"].as<glm::vec2>();
            component.size = data["BoxCollider2D"]["size"].as<glm::vec2>();
            component.trigger = data["BoxCollider2D"]["trigger"].as<bool>();
        }
        if (data["CSharpScriptComponent"]) {
            gameObject->AddComponent<CSharpScriptComponent>();
            auto &component = gameObject->GetComponent<CSharpScriptComponent>();
            for (auto s : data["CSharpScriptComponent"]["selected_scripts"]) {
                const std::string key = s.first.as<std::string>();
                const std::string str = s.second.as<std::string>();

                std::pair<std::string, std::string> keyVal;
                std::stringstream ss(str);
                std::string line;
                int ss_index = 0;
                while (getline(ss, line, '.')) {
                    if (ss_index == 0)
                        keyVal.first = line;
                    else
                        keyVal.second = line;

                    ss_index++;
                }

                if (keyVal.first == "THIS_IS_EMPTY_FUCK_YOU_LOL") keyVal.first = "";

                component.selected_scripts[key] = keyVal;
            }
        }
    }

    void Serializer::Deserialize(const std::string &path) {
        using namespace Components;
        Scene::Main->GameObjects.clear();
        std::ifstream stream(path);
        std::stringstream ss;
        ss << stream.rdbuf();

        Scene::Main->scene_file_path = path;

        yaml::Node data = yaml::Load(ss.str());
        if (!data["Scene"]) {
            std::cout << "Invalid Scene File!\n";
            return;
        }
        std::string sceneName = data["Scene"].as<std::string>();

        auto gameObjects = data["GameObjects"];

        glfwSetWindowTitle(VaultRenderer::Window::window->GetGLFWWindow(), (VaultRenderer::Window::window->title + " | " + fs::path(path).filename().string()).c_str());

        if (!gameObjects)
            return;

        for (auto entity : gameObjects) {
            auto go = entity["GameObject"];

            auto m_ID = go["ID"].as<std::string>();
            auto m_name = go["name"].as<std::string>();
            auto m_tag = go["tag"].as<std::string>();
            auto m_parent = go["parent"].as<std::string>();

            auto gameObject = GameObject::New(m_name, m_tag);
            gameObject->ID = m_ID;
            gameObject->parent = m_parent;
            gameObject->GetComponent<Transform>().ID = m_ID;

            YAMLToEntity(go, gameObject);
        }
    }
    void Serializer::DeserializeRuntime(const std::string &content) {
        using namespace Components;
        Scene::Main->GameObjects.clear();

        yaml::Node data = yaml::Load(content);
        if (!data["Scene"]) {
            std::cout << "Invalid Scene File!\n";
            return;
        }
        std::string sceneName = data["Scene"].as<std::string>();

        auto gameObjects = data["GameObjects"];

        if (!gameObjects)
            return;

        for (auto entity : gameObjects) {
            auto go = entity["GameObject"];

            auto m_ID = go["ID"].as<std::string>();
            auto m_name = go["name"].as<std::string>();
            auto m_tag = go["tag"].as<std::string>();
            auto m_parent = go["parent"].as<std::string>();

            auto gameObject = GameObject::New(m_name, m_tag);
            gameObject->ID = m_ID;
            gameObject->parent = m_parent;
            gameObject->GetComponent<Transform>().ID = m_ID;

            YAMLToEntity(go, gameObject);
        }
    }

    void Serializer::SerializeMaterial(const std::string &path, VaultRenderer::Material &material) {
        yaml::Emitter emitter;
        emitter << yaml::BeginMap;
        emitter << yaml::Key << "color" << yaml::Value << material.color;
        emitter << yaml::Key << "emissionColor" << yaml::Value << material.emissionColor;
        emitter << yaml::Key << "texUVs" << yaml::Value << material.texUVs;
        emitter << yaml::Key << "diffuse" << yaml::Value << (material.diffuse ? material.diffuse->texture_data->texture_filepath : "nullptr");
        emitter << yaml::Key << "specular" << yaml::Value << (material.specular ? material.specular->texture_data->texture_filepath : "nullptr");
        emitter << yaml::Key << "normal" << yaml::Value << (material.normal ? material.normal->texture_data->texture_filepath : "nullptr");
        emitter << yaml::Key << "height" << yaml::Value << (material.height ? material.height->texture_data->texture_filepath : "nullptr");
        emitter << yaml::Key << "roughness_map" << yaml::Value << (material.roughness_map ? material.roughness_map->texture_data->texture_filepath : "nullptr");
        emitter << yaml::Key << "metallic_map" << yaml::Value << (material.metallic_map ? material.metallic_map->texture_data->texture_filepath : "nullptr");
        emitter << yaml::Key << "ao_map" << yaml::Value << (material.ao_map ? material.ao_map->texture_data->texture_filepath : "nullptr");
        // PBR
        emitter << yaml::Key << "ao" << yaml::Value << material.ao;
        emitter << yaml::Key << "metallic" << yaml::Value << material.metallic;
        emitter << yaml::Key << "roughness" << yaml::Value << material.roughness;

        std::ofstream file(path);
        file << emitter.c_str();
    }
    void Serializer::DeserializeMaterial(const std::string &path, VaultRenderer::Material &material) {
        if (!fs::exists(path)) return;
        std::ifstream stream(path);
        std::stringstream ss;
        ss << stream.rdbuf();

        yaml::Node data = yaml::Load(ss.str());
        material.color = data["color"].as<glm::vec4>();

        if (data["emissionColor"]) {
            material.emissionColor = data["emissionColor"].as<glm::vec3>();
        }

        if (data["texUVs"]) {
            material.texUVs = data["texUVs"].as<glm::vec2>();
        }

        if (data["diffuse"].as<std::string>() != "nullptr") {
            material.SetDiffuse(data["diffuse"].as<std::string>());
        } else {
            material.diffuse.reset();
        }
        if (data["specular"].as<std::string>() != "nullptr") {
            material.SetSpecular(data["specular"].as<std::string>());
        } else {
            material.specular.reset();
        }
        if (data["normal"].as<std::string>() != "nullptr") {
            material.SetNormal(data["normal"].as<std::string>());
        } else {
            material.normal.reset();
        }
        if (data["height"].as<std::string>() != "nullptr") {
            material.SetHeight(data["height"].as<std::string>());
        } else {
            material.height.reset();
        }
        if (data["roughness_map"] && data["roughness_map"].as<std::string>() != "nullptr") {
            material.SetRoughness(data["roughness_map"].as<std::string>());
        } else {
            material.height.reset();
        }
        if (data["metallic_map"] && data["metallic_map"].as<std::string>() != "nullptr") {
            material.SetMetallic(data["metallic_map"].as<std::string>());
        } else {
            material.height.reset();
        }
        if (data["ao_map"] && data["ao_map"].as<std::string>() != "nullptr") {
            material.SetAO(data["ao_map"].as<std::string>());
        } else {
            material.height.reset();
        }

        if (data["ao"]) {
            material.ao = data["ao"].as<float>();
        }

        if (data["metallic"]) {
            material.metallic = data["metallic"].as<float>();
        }

        if (data["roughness"]) {
            material.roughness = data["roughness"].as<float>();
        }
    }

    void Serializer::CreatePrefab(const std::string &path, std::shared_ptr<GameObject> &gameObject) {
        yaml::Emitter emitter;
        emitter << yaml::BeginMap;
        emitter << yaml::Key << "Prefab" << yaml::Value << gameObject->ID;
        emitter << yaml::Key << "GameObjects" << yaml::Value << yaml::BeginSeq;

        EntityToYAML(emitter, gameObject);

        emitter << yaml::EndSeq;
        emitter << yaml::EndMap;

        std::ofstream file(path);
        file << emitter.c_str();
    }

    std::shared_ptr<GameObject> Serializer::LoadPrefab(const std::string &path) {
        using namespace Components;
        std::ifstream stream(path);
        std::stringstream ss;
        ss << stream.rdbuf();

        yaml::Node data = yaml::Load(ss.str());
        if (!data["Prefab"]) {
            std::cout << "Invalid Prefab File!\n";
            return nullptr;
        }
        std::string parent_id = data["Prefab"].as<std::string>();
        auto gameObjects = data["GameObjects"];

        if (!gameObjects)
            return nullptr;

        std::string new_parent_id = uuid::generate_uuid_v4();
        std::shared_ptr<GameObject> parent_obj = nullptr;
        for (auto entity : gameObjects) {
            auto go = entity["GameObject"];

            auto m_ID = go["ID"].as<std::string>();
            auto m_name = go["name"].as<std::string>();
            auto m_tag = go["tag"].as<std::string>();
            auto m_parent = go["parent"].as<std::string>();
            bool is_parent = false;

            if (m_ID == parent_id) {
                is_parent = true;
            }

            auto gameObject = GameObject::New(m_name, m_tag);
            gameObject->ID = is_parent ? new_parent_id : uuid::generate_uuid_v4();
            gameObject->parent = is_parent ? "NO_PARENT" : new_parent_id;
            gameObject->GetComponent<Transform>().ID = gameObject->ID;
            if (is_parent) parent_obj = gameObject;

            YAMLToEntity(go, gameObject);
        }

        return parent_obj;
    }

    void Serializer::DefaultConfigFile(const std::string &path) {
        yaml::Emitter emitter;
        emitter << yaml::BeginMap;

        emitter << yaml::Key << "main_scene" << yaml::Value << "./assets/scenes/main.vault";
        emitter << yaml::Key << "title" << yaml::Value << "Vault Engine";
        emitter << yaml::Key << "shadow_near" << yaml::Value << 1.f;
        emitter << yaml::Key << "shadow_far" << yaml::Value << 100.f;
        emitter << yaml::Key << "shadow_ortho_size" << yaml::Value << 20.f;
        emitter << yaml::Key << "HDR.exposure" << yaml::Value << 2.f;
        emitter << yaml::Key << "Renderer.Bloom" << yaml::Value << true;
        emitter << yaml::Key << "usePBR" << yaml::Value << false;

        std::ofstream file(path);
        file << emitter.c_str();
    }

    void Serializer::LoadConfigFile(const std::string &path) {
        if (!fs::exists(path)) DefaultConfigFile(path);

        std::ifstream stream(path);
        std::stringstream ss;
        ss << stream.rdbuf();

        yaml::Node data = yaml::Load(ss.str());

        if (data["main_scene"]) {
            config.main_scene = data["main_scene"].as<std::string>();
        }

        if (data["title"]) {
            config.title = data["title"].as<std::string>();
        }

        if (data["shadow_near"]) {
            config.shadow_near = data["shadow_near"].as<float>();
        }

        if (data["shadow_far"]) {
            config.shadow_far = data["shadow_far"].as<float>();
        }

        if (data["shadow_ortho_size"]) {
            config.shadow_ortho_size = data["shadow_ortho_size"].as<float>();
        }

        if (data["HDR.exposure"]) {
            config.HDR.exposure = data["HDR.exposure"].as<float>();
        }

        if (data["Renderer.Bloom"]) {
            VaultRenderer::Window::Renderer.Bloom = data["Renderer.Bloom"].as<bool>();
        }

        if (data["usePBR"]) {
            Runtime::instance->usePBR = data["usePBR"].as<bool>();
        }

        if (data["PostProcessing.GlobalBloom"]) {
            PostProcessing::GlobalBloom = data["PostProcessing.GlobalBloom"].as<bool>();
        }

        if (data["PostProcessing.BloomThreshold"]) {
            PostProcessing::BloomThreshold = data["PostProcessing.BloomThreshold"].as<float>();
        }

        if (data["PostProcessing.BloomMultiplier"]) {
            PostProcessing::BloomMultiplier = data["PostProcessing.BloomMultiplier"].as<glm::vec3>();
        }
    }

    void Serializer::SaveConfigFile(const std::string &path) {
        yaml::Emitter emitter;
        emitter << yaml::BeginMap;

        // Main
        emitter << yaml::Key << "main_scene" << yaml::Value << config.main_scene;
        emitter << yaml::Key << "title" << yaml::Value << config.title;

        // Shadows
        emitter << yaml::Key << "shadow_near" << yaml::Value << config.shadow_near;
        emitter << yaml::Key << "shadow_far" << yaml::Value << config.shadow_far;
        emitter << yaml::Key << "shadow_ortho_size" << yaml::Value << config.shadow_ortho_size;

        // HDR
        emitter << yaml::Key << "HDR.exposure" << yaml::Value << config.HDR.exposure;

        // Renderer
        emitter << yaml::Key << "Renderer.Bloom" << yaml::Value << VaultRenderer::Window::Renderer.Bloom;
        emitter << yaml::Key << "usePBR" << yaml::Value << Runtime::usePBR;

        // Post Processing
        emitter << yaml::Key << "PostProcessing.GlobalBloom" << yaml::Value << PostProcessing::GlobalBloom;
        emitter << yaml::Key << "PostProcessing.BloomThreshold" << yaml::Value << PostProcessing::BloomThreshold;
        emitter << yaml::Key << "PostProcessing.BloomMultiplier" << yaml::Value << PostProcessing::BloomMultiplier;

        std::ofstream file(path);
        file << emitter.c_str();
    }
} // namespace Engine