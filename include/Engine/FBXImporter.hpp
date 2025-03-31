#pragma once

// thanks to void (github link here) for this code cause im fucking lazy to rewrite the shitty animation system
// maybe i wont use it

#include "Engine/Model.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include <Renderer/Mesh.hpp>
#include <memory>
#include <ofbx/ofbx.hpp>

#define VMDL_BONE_NO_PARENT 999999

namespace VaultRenderer {
    struct ImportedMesh {
        std::string name;
        std::shared_ptr<VaultRenderer::Mesh> mesh;
        uint32_t first_bone_id = -1;
        const ofbx::Object *obj;
    };

    struct ImportedBone {
        uint64_t ID;
        uint64_t ParentID;
        std::string Name;
        glm::mat4 GlobalInverseBindPoseMatrix;
        glm::mat4 GlobalBindPoseMatrix;

        int16_t index;
        int16_t parent_index = -999;
    };

    struct ImportedSkin {
        float weights[4];
        int16_t joints[4];
        int count = 0;
    };

    struct ImportedAnimationBoneKeyFrame {
        glm::vec3 position;
        glm::vec3 rotation;
    };

    struct ImportedAnimationFrameTransform {
        glm::mat4 transform;
    };

    struct ImportedAnimation {
        std::string name;
        double duration;
        float fps;
        std::vector<std::vector<ImportedAnimationFrameTransform>> tracks;
        uint32_t animation_id;
    };

    struct FBXLoadResult {
        std::vector<std::shared_ptr<VaultRenderer::Mesh>> meshes;
        std::vector<ImportedAnimation> animations;
        bool isSuccessful = false;
        std::string message;
        std::string name;
    };

    struct Skeleton {
        struct Bone {
            std::string m_BoneName;

            uint32_t m_Index;
            uint32_t m_ParentIndex = VMDL_BONE_NO_PARENT;
            std::vector<uint32_t> m_ChildrenIndices;

            glm::mat4 m_BindPoseMatrix;
            // inverse bind pose matrix in MODEL space
            glm::mat4 m_InverseBindPoseMatrix;

            // where the bone is in MODEL space.
            glm::mat4 m_BoneTransform;

            // Where the bone is relative to it's parent.
            glm::mat4 m_LocalBoneTransform;

            Skeleton *m_Skeleton;
        };

        std::vector<Bone> m_Bones;
    };

    class FBXImporter {
    public:
        FBXLoadResult ImportModel(std::string filePath);

    private:
        void ProcessScene(ofbx::IScene *scene);

        void GatherMeshes(const ofbx::Object &parent);
        void GatherBones(const ofbx::Object &parent);
        void GatherAnimations();
        void GatherKeyframes(ImportedAnimation &animation);

        void SortBones();
        void ProcessBones();
        void InsertBone(const ofbx::Object *node);
        void LinkMeshToBones(ImportedMesh &mesh, int32_t bone_id);
        void WriteSkeletons();
        std::vector<uint32_t> GetChildBoneIndices(ImportedBone &parent);

        std::shared_ptr<VaultRenderer::Mesh> ProcessMeshNode(const ofbx::Mesh *mesh, std::vector<ImportedSkin> skins, bool hasValidSkins);

        std::vector<ImportedSkin> GetSkinsForMesh(const ofbx::Mesh *mesh, bool &hasValidSkins);
        glm::mat4 GetBindPoseMatrix(ImportedBone &bone);

        glm::mat4 OFBXMatrixToVoidMatrix(ofbx::DMatrix matrix);

        glm::mat4 FixOrientation(glm::mat4 m);
        const ofbx::Mesh *GetAnyMeshFromBone(const ofbx::Object *node, int32_t bone_id);

        void NormalizeBoneScale(glm::mat4 &matrix, bool applySceneScale);

        std::shared_ptr<char> GetTextureDataFromMaterial(const ofbx::Material *material, ofbx::Texture::TextureType textureType, uint32_t &outTextureSize);

        std::string DataviewToString(ofbx::DataView *view);

        void ComputeTangentsAndBitangents(std::vector<VaultRenderer::Vertex> &vertexBuffer, const std::vector<uint16_t> &indexBuffer);

    private:
        std::vector<ImportedMesh> m_Meshes;

        std::vector<ImportedSkin> m_Skins;
        std::vector<ImportedBone> m_Bones;
        std::vector<ImportedAnimation> m_Animations;

        ofbx::IScene *m_Scene;
    };
} // namespace VaultRenderer