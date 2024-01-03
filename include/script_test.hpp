#pragma once
#include <iostream>

class Script {
public:
    Script() = default;

    virtual void Update() {
        std::cout << "base update\n";
    }
    virtual void Start() {
        std::cout << "base start\n";
    };
};