#include "Engine/Components/Camera.hpp"
#include "Engine/Components/DirectionalLight.hpp"
#include "Engine/Runtime.hpp"
#include "Renderer/Window.hpp"
#include "glm/fwd.hpp"
#include <Engine/CascadedShadowMap.hpp>

using namespace VaultRenderer;
namespace Engine {
    CascadedShadowMap::CascadedShadowMap(const int depthMapRes, float cameraFarPlane) : res(depthMapRes) {
        m_ShadowCascadeLevels = {cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f};
        shader = std::make_unique<Shader>("./shaders/cascaded_shadowmap.glsl");

        GenerateFBO();
    }

    void CascadedShadowMap::GenerateFBO() {
        glGenFramebuffers(1, &m_FBO);

        glGenTextures(1, &m_LightDepthMaps);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_LightDepthMaps);
        glTexImage3D(
            GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, res, res, int(m_ShadowCascadeLevels.size()) + 1,
            0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        constexpr float bordercolor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_LightDepthMaps, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "[ERROR] Framebuffer is not complete!";
            throw 0;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenBuffers(1, &m_MatricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_MatricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void CascadedShadowMap::BindMap(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_LightDepthMaps);
    }

    glm::mat4 CascadedShadowMap::GetLightSpaceMatrix(Components::Camera &camera, float near, float far, glm::vec3 lightDir) {
        const float aspect = (float)res / (float)res;
        const auto proj = glm::perspective(glm::radians(camera.fov), aspect, near, far);

        const auto corners = Runtime::instance->GetFrustumCornersWorldSpace(proj, camera.view);

        glm::vec3 center = glm::vec3(0, 0, 0);
        for (const auto &v : corners) {
            center += glm::vec3(v);
        }
        center /= corners.size();

        const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();
        for (const auto &v : corners) {
            const auto trf = lightView * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        // Tune this parameter according to the scene
        constexpr float zMult = 10.0f;
        if (minZ < 0) {
            minZ *= zMult;
        } else {
            minZ /= zMult;
        }
        if (maxZ < 0) {
            maxZ /= zMult;
        } else {
            maxZ *= zMult;
        }

        const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        return lightProjection * lightView;
    }

    std::vector<glm::mat4> CascadedShadowMap::GetLightSpaceMatrices(Components::Camera &camera, glm::vec3 lightDir) {
        std::vector<glm::mat4> ret;
        // const auto corners = Runtime::instance->GetFrustumCornersWorldSpace(camera.projection, camera.view);
        std::cout << m_ShadowCascadeLevels.size() + 1 << " IS THE SIZE\n";
        for (size_t i = 0; i < m_ShadowCascadeLevels.size() + 1; ++i) {
            // GetLightSpaceMatrix(camera, camera.near, camera.far, lightDir);
            if (i == 0) {
                ret.push_back(GetLightSpaceMatrix(camera, camera.near, m_ShadowCascadeLevels[i], lightDir));
            } else if (i < m_ShadowCascadeLevels.size()) {
                ret.push_back(GetLightSpaceMatrix(camera, m_ShadowCascadeLevels[i - 1], m_ShadowCascadeLevels[i], lightDir));
            } else {
                ret.push_back(GetLightSpaceMatrix(camera, m_ShadowCascadeLevels[i - 1], camera.far, lightDir));
            }
        }
        return ret;
    }

    void CascadedShadowMap::BindToShader(VaultRenderer::Shader &shader) {
        if (!lightDir) {
            auto &light = Scene::Main->EntityRegistry.get<Components::DirectionalLight>(Scene::Main->EntityRegistry.view<Components::DirectionalLight>().back());
            lightDir = &light.transform->position;
        }

        shader.Bind();
        shader.SetUniform1i("cascadeCount", m_ShadowCascadeLevels.size());
        shader.SetUniform3f("c_ShadowMapLightDir", lightDir->x, lightDir->y, lightDir->z);
        for (size_t i = 0; i < m_ShadowCascadeLevels.size(); ++i) {
            shader.SetUniform1f(("cascadePlaneDistances[" + std::to_string(i) + "]").c_str(), m_ShadowCascadeLevels[i]);
        }
    }

    void CascadedShadowMap::BindForDrawing() {
        static bool inited = false;
        auto &light = Scene::Main->EntityRegistry.get<Components::DirectionalLight>(Scene::Main->EntityRegistry.view<Components::DirectionalLight>().back());
        lightDir = &light.transform->position;

        const auto lightMatrices = GetLightSpaceMatrices(*Scene::Main->main_camera_object, *lightDir);
        std::cout << "erm yes??\n";
        glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesUBO);
        for (size_t i = 0; i < lightMatrices.size(); ++i) {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        shader->Bind();
        shader->SetUniform1i("cascadeCount", m_ShadowCascadeLevels.size());
        shader->SetUniform3f("c_ShadowMapLightDir", lightDir->x, lightDir->y, lightDir->z);
        for (size_t i = 0; i < m_ShadowCascadeLevels.size(); ++i) {
            shader->SetUniform1f(("cascadePlaneDistances[" + std::to_string(i) + "]").c_str(), m_ShadowCascadeLevels[i]);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_TEXTURE_2D_ARRAY, m_LightDepthMaps, 0);
        glViewport(0, 0, res, res);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
    }

    void CascadedShadowMap::UnbindDrawing() {
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glDisable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // namespace Engine