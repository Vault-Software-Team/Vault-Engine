#pragma once
#include <Engine/Scene.hpp>
#include <yaml-cpp/yaml.h>
#include <Renderer/Mesh.hpp>

namespace yaml = YAML;

namespace Engine {
    class Serializer {
    public:
        inline static std::string scheduled_scene_path = "";

        static void Serialize(std::unique_ptr<Scene> &scene, const std::string &path);
        static void SerializeRuntime(const std::string &path);
        static void EntityToYAML(yaml::Emitter &emitter, std::shared_ptr<GameObject> &gameObject);
        static void YAMLToEntity(yaml::Node &data, std::shared_ptr<GameObject> &gameObject);
        static void Deserialize(const std::string &path);
        static void DeserializeRuntime(const std::string &path);
        static void SerializeMaterial(const std::string &path, VaultRenderer::Material &material);
        static void DeserializeMaterial(const std::string &path, VaultRenderer::Material &material);
    };
} // namespace Engine