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
    class AssimpGLMHelpers {
    public:
        static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4 &from) {
            glm::mat4 to;
            // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
            to[0][0] = from.a1;
            to[1][0] = from.a2;
            to[2][0] = from.a3;
            to[3][0] = from.a4;
            to[0][1] = from.b1;
            to[1][1] = from.b2;
            to[2][1] = from.b3;
            to[3][1] = from.b4;
            to[0][2] = from.c1;
            to[1][2] = from.c2;
            to[2][2] = from.c3;
            to[3][2] = from.c4;
            to[0][3] = from.d1;
            to[1][3] = from.d2;
            to[2][3] = from.d3;
            to[3][3] = from.d4;
            return to;
        }

        static inline glm::vec3 GetGLMVec(const aiVector3D &vec) {
            return glm::vec3(vec.x, vec.y, vec.z);
        }

        static inline glm::quat GetGLMQuat(const aiQuaternion &pOrientation) {
            return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
        }
    };

    class DLL_API Model {
    public:
        Model(const std::string &path);
        Model(const char *path);

    private:
        std::string directory;
        std::shared_ptr<GameObject> parent;

        std::map<std::string, VaultRenderer::BoneInfo> m_BoneInfoMap; //
        int m_BoneCounter = 0;

        auto &GetBoneInfoMap() { return m_BoneInfoMap; }
        int &GetBoneCount() { return m_BoneCounter; }
        void SetVertexBoneDataToDefault(VaultRenderer::Vertex &vertex);

        void SetVertexBoneData(VaultRenderer::Vertex &vertex, int boneID, float weight);

        void ExtractBoneWeightForVertices(std::vector<VaultRenderer::Vertex> &vertices, aiMesh *mesh, const aiScene *scene);

        void loadModel(const std::string &path);
        void processNode(aiNode *node, const aiScene *scene);
        void processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<VaultRenderer::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);
    };
} // namespace Engine