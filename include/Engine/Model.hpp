#pragma once
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include <glm/matrix.hpp>
#include <unordered_map>
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
#include <glm/gtx/quaternion.hpp>
#include <vector>

namespace Engine {
    class AssimpGLMHelpers {
    public:
        static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4 &from) {
            glm::mat4 to;
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
        struct GlobalBoneMap {
            std::map<std::string, VaultRenderer::BoneInfo> map;
            int bone_counter = 0;
        };

        static DLL_API std::map<std::string, GlobalBoneMap> GlobalBoneMaps;

        int index = 0;
        Model(const std::string &path);
        Model(const char *path);
        auto &GetBoneInfoMap() { return m_BoneInfoMap; }
        int &GetBoneCount() { return m_BoneCounter; }
        void SetVertexBoneDataToDefault(VaultRenderer::Vertex &vertex);

        void SetVertexBoneData(VaultRenderer::Vertex &vertex, int boneID, float weight);

        void ExtractBoneWeightForVertices(std::vector<VaultRenderer::Vertex> &vertices, aiMesh *mesh, const aiScene *scene);

    private:
        std::string path;
        std::string directory;
        std::shared_ptr<GameObject> parent;

        std::map<std::string, VaultRenderer::BoneInfo> m_BoneInfoMap; //
        int m_BoneCounter = 0;

        void loadModel(const std::string &path);
        void processNode(aiNode *node, const aiScene *scene);
        void processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<VaultRenderer::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);
    };

    class DLL_API ModelMesh {
    public:
        struct Info {
            ModelMesh *ref;
            int count;
        };
        static std::unordered_map<std::string, Info> LoadedModels;

        ModelMesh(const std::string &path);
        ModelMesh(const char *path);
        ~ModelMesh();
        auto &GetBoneInfoMap() { return m_BoneInfoMap; }
        int &GetBoneCount() { return m_BoneCounter; }
        void SetVertexBoneDataToDefault(VaultRenderer::Vertex &vertex);

        void SetVertexBoneData(VaultRenderer::Vertex &vertex, int boneID, float weight);

        void ExtractBoneWeightForVertices(std::vector<VaultRenderer::Vertex> &vertices, aiMesh *mesh, const aiScene *scene);

        std::vector<VaultRenderer::Mesh> meshes;
        // std::vector<glm::mat4> mesh_transforms;
        glm::mat4 modelTransform = glm::mat4(1.0);
        VaultRenderer::Mesh *GetMeshValueByIndex(int index);

        std::string path;
        std::string directory;

        std::map<std::string, VaultRenderer::BoneInfo> m_BoneInfoMap; //
        int m_BoneCounter = 0;

        void loadModel(const std::string &path);
        void processNode(aiNode *node, const aiScene *scene);
        VaultRenderer::Mesh &processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<VaultRenderer::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);
    };

    struct KeyframePos {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyframeRot {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyframeScale {
        glm::vec3 scale;
        float timeStamp;
    };
    struct AssimpNodeData {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    class Bone {
    private:
        std::vector<KeyframePos> positions;
        std::vector<KeyframeRot> rotations;
        std::vector<KeyframeScale> scales;
        int numPos;
        int numRots;
        int numScales;

        std::string name;
        int id;

    public:
        Bone(const std::string &name, int ID, const aiNodeAnim *channel);
        Bone() = default;

        glm::mat4 local_transform;
        glm::vec3 m_pos{0, 0, 0}, m_rot{0, 0, 0}, m_scal{1, 1, 1};
        void Update(float anim_time) {
            glm::mat4 translation = InterPos(anim_time);
            glm::mat4 rotation = InterRot(anim_time);
            glm::mat4 scale = InterScal(anim_time);
            local_transform = translation * rotation * scale;
        }

        void SetLocalTransform();

        glm::mat4 GetLocalTransform() { return local_transform; }
        std::string GetName() const { return name; }
        int GetID() { return id; }

        int GetPositionIndex(float anim_time);
        int GetRotationIndex(float anim_time);
        int GetScaleIndex(float anim_time);

    private:
        float scale_factor(float last, float next, float anim_time);

        glm::mat4 InterPos(float anim_time);
        glm::mat4 InterRot(float anim_time);
        glm::mat4 InterScal(float anim_time);
    };

    class DLL_API Animation {
    public:
        const std::string animation_path;

        Animation() = default;
        Animation(const std::string &animationPath, Model::GlobalBoneMap &bone_map);
        ~Animation();

        std::shared_ptr<Bone> FindBone(const std::string &name);

        inline float GetTicksPerSecond() { return ticks_per_sec; }
        inline float GetDuration() { return duration; }
        inline const AssimpNodeData &GetRootNode() { return root_node; }
        inline std::map<std::string, VaultRenderer::BoneInfo> &GetIDMap() { return bonemap.map; }

        inline auto &GetBoneInfoMap() { return bonemap.map; }
        inline int &GetBoneCount() { return bonemap.bone_counter; }

    private:
        void ReadMissingBones(const aiAnimation *animation);
        void ReadHeirarchyData(AssimpNodeData &dest, const aiNode *src);

        float duration;
        int ticks_per_sec;
        std::unordered_map<std::string, std::shared_ptr<Bone>> bones;
        AssimpNodeData root_node;
        Model::GlobalBoneMap bonemap;
    };

    class DLL_API Animator {
    public:
        Animation *curr_anim;

        Animator(Animation *anim);
        void UpdateAnimation(float dt);
        void UpdateTransforms();
        void PlayAnimation(Animation *anim);
        void CalculateBoneTransform(const AssimpNodeData *node, const glm::mat4 &parent);
        void LocalTransformCalculateBoneTransform(const AssimpNodeData *node, const glm::mat4 &parent);
        void SetCurrentAnimation(Animation *anim);
        std::vector<glm::mat4> &GetFinalBoneMatrices();

    private:
        std::vector<glm::mat4> finalBoneMatrices;
        float curr_time;
        float timestep;
    };
} // namespace Engine