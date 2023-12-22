#include <Engine/GameObject.hpp>

namespace Engine {
    std::unique_ptr<GameObject> GameObject::New(const std::string &name, const std::string &tag) {
        return std::make_unique<GameObject>(name, tag);
    }
} // namespace Engine