#include <Engine/Components/Camera.hpp>
#include <Engine/Scene.hpp>

namespace Engine::Components {
    void Camera::UpdateMatrix() {
        Transform &transform = Scene::Main->EntityRegistry.get<Transform>(entity);

        view = glm::lookAt(transform.position, transform.position + transform.rotation, up);
        projection = glm::perspective(glm::radians(fov), (float)width / (float)height, near, far);
    }

    void Camera::BindToShader(VaultRenderer::Shader &shader) {
        shader.Bind();
        shader.SetUniformMat4("camera_view", view);
        shader.SetUniformMat4("camera_projection", projection);
    }
} // namespace Engine::Components