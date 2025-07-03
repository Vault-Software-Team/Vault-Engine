#pragma once
#include <Engine/Scene.hpp>
#include <yaml-cpp/yaml.h>
#include <Renderer/Mesh.hpp>

namespace yaml = YAML;

namespace Engine {
    struct Config {
        std::string main_scene = "";
        std::string title = "";
        float shadow_near = 1.f;
        float shadow_far = 100.f;
        float shadow_ortho_size = 20.f;

        struct {
            float exposure = 2.0f;
        } HDR;
    };

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
        static std::string CreatePrefabInMemory(const std::string &path, std::shared_ptr<GameObject> &gameObject);
        static std::shared_ptr<GameObject> LoadPrefab(const std::string &path);
        static std::shared_ptr<GameObject> LoadPrefabFromString(const std::string &prefab_data);

        // Config
        inline static Config config;

        static void LoadConfigFile(const std::string &path);
        static void SaveConfigFile(const std::string &path);
        static void DefaultConfigFile(const std::string &path);
    };
} // namespace Engine