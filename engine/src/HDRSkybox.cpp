#include "Engine/SimpleCalls.hpp"
#include <Engine/HDRSkybox.hpp>
#include <stb_image/stb_image.h>
#include <iostream>
#include <Renderer/Window.hpp>

namespace VaultRenderer {

    HDRSkybox::HDRSkybox(const std::string &tex, Shader *equirectToCubemapShader, Shader *hdriSkyboxShader, Shader *irrShader, Shader *prefilterShader, Shader *brdfShader, const int res)
        : equirectToCubemapShader(equirectToCubemapShader), hdriSkyboxShader(hdriSkyboxShader), irrShader(irrShader), prefilterShader(prefilterShader), brdfShader(brdfShader), res(res) {
        hdr_texture = std::make_unique<Texture>(tex, TEXTURE_HDRI);

        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, res, res);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        std::vector<glm::mat4> captureViews = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) //
        };

        // Env Cubemap
        glGenTextures(1, &envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, res, res, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Irradiance (have to create separate fbo for some reason)
        glGenFramebuffers(1, &icaptureFBO);
        glGenRenderbuffers(1, &icaptureRBO);

        glGenTextures(1, &irradianceMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
                         GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindFramebuffer(GL_FRAMEBUFFER, icaptureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, icaptureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

        irrShader->Bind();
        irrShader->SetUniform1i("environmentMap", 0);
        irrShader->SetUniformMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, icaptureFBO);
        for (unsigned int i = 0; i < 6; ++i) {
            irrShader->SetUniformMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            SimpleCalls::RenderCube();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Prefilter
        glGenFramebuffers(1, &pcaptureFBO);
        glGenRenderbuffers(1, &pcaptureRBO);

        glGenTextures(1, &prefilterMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        prefilterShader->Bind();
        prefilterShader->SetUniform1i("environmentMap", 0);
        prefilterShader->SetUniformMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        glBindFramebuffer(GL_FRAMEBUFFER, pcaptureFBO);
        uint32_t maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            // reisze framebuffer according to mip-level size.
            unsigned int mipWidth = 128 * std::pow(0.5, mip);
            unsigned int mipHeight = 128 * std::pow(0.5, mip);
            glBindRenderbuffer(GL_RENDERBUFFER, pcaptureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            prefilterShader->SetUniform1f("roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i) {
                prefilterShader->SetUniformMat4("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                SimpleCalls::RenderCube();
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // BRDF
        glGenFramebuffers(1, &bcaptureFBO);
        glGenRenderbuffers(1, &bcaptureRBO);
        glGenTextures(1, &brdfLUT);

        // pre-allocate enough memory for the LUT texture.
        glBindTexture(GL_TEXTURE_2D, brdfLUT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, bcaptureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, bcaptureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);

        glViewport(0, 0, 512, 512);
        glDisable(GL_BLEND);
        brdfShader->Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SimpleCalls::RenderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        RenderEnvCubemap();
    }

    void HDRSkybox::RenderEnvCubemap() {
        static int counter = 0;

        if (counter >= 4) return;
        if (counter < 4) counter++;

        std::cout << "PAST HERE \n";

        // ENV CUBEMAP
        equirectToCubemapShader->Bind();
        equirectToCubemapShader->SetUniform1i("equirectangularMap", 0);
        equirectToCubemapShader->SetUniformMat4("projection", captureProjection);
        hdr_texture->Bind(0);
        glViewport(0, 0, res, res); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i) {
            equirectToCubemapShader->SetUniformMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            SimpleCalls::RenderCube();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // IRRADIANCE
        glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, icaptureFBO);
        irrShader->Bind();
        irrShader->SetUniform1i("environmentMap", 0);
        irrShader->SetUniformMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        for (unsigned int i = 0; i < 6; ++i) {
            irrShader->SetUniformMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            SimpleCalls::RenderCube();
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // PREFILTER
        prefilterShader->Bind();
        prefilterShader->SetUniform1i("environmentMap", 0);
        prefilterShader->SetUniformMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        glBindFramebuffer(GL_FRAMEBUFFER, pcaptureFBO);
        uint32_t maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            // reisze framebuffer according to mip-level size.
            unsigned int mipWidth = 128 * std::pow(0.5, mip);
            unsigned int mipHeight = 128 * std::pow(0.5, mip);
            glBindRenderbuffer(GL_RENDERBUFFER, pcaptureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            prefilterShader->SetUniform1f("roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i) {
                prefilterShader->SetUniformMat4("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                SimpleCalls::RenderCube();
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // BRDF
        glBindFramebuffer(GL_FRAMEBUFFER, bcaptureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, bcaptureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);

        glViewport(0, 0, 512, 512);
        glDisable(GL_BLEND);

        brdfShader->Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SimpleCalls::RenderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
        // glBindFramebuffer(GL_FRAMEBUFFER, icaptureFBO);
        // irrShader->Bind();
        // hdr_texture->Bind(0);
        // for (unsigned int i = 0; i < 6; ++i) {
        //     irrShader->SetUniformMat4("view", captureViews[i]);
        //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        //                            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //     SimpleCalls::RenderCube();
        // }
        // glDepthFunc(GL_LESS);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void HDRSkybox::BindIrradianceMap(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    }

    void HDRSkybox::BindEnvCubemap(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    }

    void HDRSkybox::BindPrefilterMap(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    }

    void HDRSkybox::BindBRDFLUT(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, brdfLUT);
    }

    void HDRSkybox::RenderSkybox(Shader &skyboxShader, Engine::Components::Camera &camera) {
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Bind();
        skyboxShader.SetUniformMat4("view", camera.view);
        skyboxShader.SetUniformMat4("projection", camera.projection);
        BindIrradianceMap(0);
        skyboxShader.SetUniform1i("environmentMap", 0);
        SimpleCalls::RenderCube();
        glDepthFunc(GL_LESS);
    }
} // namespace VaultRenderer