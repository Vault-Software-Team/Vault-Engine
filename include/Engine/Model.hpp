#pragma once
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <Renderer/Shader.hpp>
#include <memory>
#include <Renderer/Texture.hpp>
#include <Renderer/Mesh.hpp>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <Engine/GameObject.hpp>
#include <vector>

namespace Engine {
    class DLL_API Model {
    public:
        Model(const std::string &path);
        Model(const char *path);

    private:
        std::string directory;
        std::shared_ptr<GameObject> parent;

        void loadModel(const std::string &path);
        void processNode(aiNode *node, const aiScene *scene);
        void processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<VaultRenderer::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);
    };
} // namespace Engine