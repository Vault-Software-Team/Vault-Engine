#include <Engine/SceneSerialization.hpp>
#include <Engine/GameObject.hpp>
#include <fstream>
#include <Engine/Components/IncludeComponents.hpp>
#include <iostream>

namespace YAML {
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
            rhs.y = node[0].as<float>();
            rhs.z = node[0].as<float>();

            return true;
        }
    };
} // namespace YAML

namespace Engine {
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

    Serializer::Serializer(std::unique_ptr<Scene> &scene) : scene(scene) {
    }

    void Serializer::Serialize(const std::string &path) {
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
            emitter << yaml::Key << "scale" << yaml::Value << component.scale;
            emitter << yaml::Key << "y_offset" << yaml::Value << component.y_offset;

            emitter << yaml::EndMap;
        }

        if (gameObject->HasComponent<MeshRenderer>()) {
            emitter << yaml::Key << "MeshRenderer";
            emitter << yaml::BeginMap;

            auto &component = gameObject->GetComponent<MeshRenderer>();
            emitter << yaml::Key << "mesh_type" << yaml::Value << component.mesh_type;

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

            emitter << yaml::EndMap;
        }

        emitter << YAML::EndMap;

        emitter << yaml::EndMap;
    }
    void Serializer::SerializeRuntime(const std::string &path) {
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
        }

        if (data["MeshRenderer"]) {
            gameObject->AddComponent<MeshRenderer>();
            auto &component = gameObject->GetComponent<MeshRenderer>();
            component.SetMeshType((MeshType)data["MeshRenderer"]["mesh_type"].as<int>());
        }
        if (data["Camera"]) {
            gameObject->AddComponent<Camera>();
            auto &component = gameObject->GetComponent<Camera>();
            component.fov = data["Camera"]["fov"].as<float>();
            component.near = data["Camera"]["near"].as<float>();
            component.far = data["Camera"]["far"].as<float>();
            component.main_camera = data["Camera"]["main_camera"].as<bool>();
        }
    }

    void Serializer::Deserialize(const std::string &path) {
        using namespace Components;
        std::ifstream stream(path);
        std::stringstream ss;
        ss << stream.rdbuf();

        yaml::Node data = yaml::Load(ss.str());
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
            gameObject->GetComponent<Transform>().ID = m_ID;

            YAMLToEntity(go, gameObject);
        }
    }
    void Serializer::DeserializeRuntime(const std::string &path) {
    }
} // namespace Engine