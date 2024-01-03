#include <iostream>
#include "script.hpp"

extern "C" {
Script *create_script() {
    return new MyScript();
}
}

void MyScript::Update() {
    std::cout << "MyScript Update\n";
}

void MyScript::Start() {
    std::cout << "MyScript Start";
}