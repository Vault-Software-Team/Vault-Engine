#pragma once
#include <Engine/Scene.hpp>
#include <yaml-cpp/yaml.h>

namespace yaml = YAML;

namespace Engine {
    class Serializer {
    private:
        std::unique_ptr<Scene> &scene;

    public:
        Serializer(std::unique_ptr<Scene> &scene);

        void Serialize(const std::string &path);
        void SerializeRuntime(const std::string &path);
        void EntityToYAML(yaml::Emitter &emitter, std::shared_ptr<GameObject> &gameObject);
        void YAMLToEntity(yaml::Node &data, std::shared_ptr<GameObject> &gameObject);
        void Deserialize(const std::string &path);
        void DeserializeRuntime(const std::string &path);
    };
} // namespace Engine