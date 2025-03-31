#include <Engine/PostProcessing.hpp>

using namespace Engine;

bool PostProcessing::GlobalBloom;
float PostProcessing::BloomThreshold;
glm::vec3 PostProcessing::BloomMultiplier;
bool PostProcessing::PBR_IBL;

void PostProcessing::BindToShader(VaultRenderer::Shader &shader, const std::string &config_PostProcessing) {
    shader.Bind();
    shader.SetUniform1i("config_PostProcessing.GlobalBloom", GlobalBloom);
    shader.SetUniform1i("config_PostProcessing.PBR_IBL", PBR_IBL);
    shader.SetUniform1f("config_PostProcessing.BloomThreshold", BloomThreshold);
    shader.SetUniform3f("config_PostProcessing.BloomMultiplier", BloomMultiplier.x, BloomMultiplier.y, BloomMultiplier.z);
}
