#pragma once
#include <Engine/GameObject.hpp>
#include <string>

namespace Editor {
    class GUI {
    public:
        static void SetNameIcon(std::string &txt, std::shared_ptr<Engine::GameObject> &gameObject);
        static void Hierarchy();
    };
} // namespace Editor