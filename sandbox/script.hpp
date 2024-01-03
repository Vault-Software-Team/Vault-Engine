#pragma once

#include "../include/script_test.hpp"

extern "C" {
Script *create_script();
}

class MyScript : public Script {
public:
    void Update() override;
    void Start() override;
};