#include <iostream>
#include <Renderer/Window.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/Texture.hpp>
#include <imgui/imgui.h>
#include <Engine/Scene.hpp>

int main() {
    using namespace VaultRenderer;
    Window window(800, 600, "Vault Engine");

    std::vector<Vertex> vertices = {
        // {glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1, 1)},
        // {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1, 0)},
        // {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0, 0)},
        // {glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0, 1)},

        {glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec2(5.0f, 0.0f)},
        {glm::vec3(0.5f, 0.0f, -0.5f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, 0.0f, 0.5f), glm::vec2(5.0f, 0.0f)},
        {glm::vec3(0.0f, 0.8f, 0.0f), glm::vec2(2.5f, 5.0f)}};

    std::vector<uint32_t> indices = {
        // 0, 1, 3, // first triangle
        // 1, 2, 3
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4};

    Shader shader("../shaders/default.glsl");
    Mesh square(vertices, indices);
    Texture texture("../assets/k7.png");

    shader.Bind();
    texture.Bind(0);
    shader.SetUniform1i("texture_diffuse", 0);

    float rotation = 0.5f;

    double previousTime = glfwGetTime();

    window.Run([&] {
        window.SetClearColor(0x000000);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.f, 1.f, 0.f));
        view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)(800.f / 600.f), 0.01f, 100.0f);

        double currentTime = glfwGetTime();
        if (currentTime - previousTime >= 1 / 60) {
            rotation += 0.5f;
            previousTime = currentTime;
        }

        shader.SetUniformMat4("transformModel", model);
        shader.SetUniformMat4("view", view);
        shader.SetUniformMat4("projection", projection);

        square.Draw(shader);

        if (ImGui::Begin("test")) {
            ImGui::Text("Hello filthy world!");
            ImGui::End();
        }
    });

    return 0;
}