#pragma once
#include <Engine/Scene.hpp>
#include <yaml-cpp/yaml.h>
#include <Renderer/Mesh.hpp>

namespace yaml = YAML;

namespace Engine {
    class Serializer {
    public:
        inline static std::string scheduled_scene_path = "";

        static void Serialize(std::shared_ptr<Scene> &scene, const std::string &path);
        static std::string SerializeRuntime();
        static void EntityToYAML(yaml::Emitter &emitter, std::shared_ptr<GameObject> &gameObject);
        static void YAMLToEntity(yaml::Node &data, std::shared_ptr<GameObject> &gameObject);
        static void Deserialize(const std::string &path);
        static void DeserializeRuntime(const std::string &content);
        static void SerializeMaterial(const std::string &path, VaultRenderer::Material &material);
        static void DeserializeMaterial(const std::string &path, VaultRenderer::Material &material);
        static void CreatePrefab(const std::string &path, std::shared_ptr<GameObject> &gameObject);
        static void LoadPrefab(const std::string &path);
    };
} // namespace Engine