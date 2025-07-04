#include "Engine/SceneSerialization.hpp"
#include "Renderer/Mesh.hpp"
#include "assimp/material.h"
#include "assimp/matrix4x4.h"
#include "assimp/postprocess.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "uuid.hpp"
#include <Engine/Model.hpp>
#include <Engine/Components/MeshRenderer.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace Engine {
    ModelMesh::ModelMesh(const std::string &path) : path(path) {
        if (LoadedModels.find(path) != LoadedModels.end()) {
            auto &info = LoadedModels[path];
            info.count++;
        } else {
            LoadedModels[path] = {this, 1};
        }
        loadModel(path);
    }

    ModelMesh::ModelMesh(const char *path) : path(path) {
        if (LoadedModels.find(path) != LoadedModels.end()) {
            auto &info = LoadedModels[path];
            info.count++;
        } else {
            LoadedModels[path] = {this, 1};
        }
        loadModel(path);
    }

    ModelMesh::~ModelMesh() {
        if (LoadedModels.find(path) != LoadedModels.end()) {
            auto &info = LoadedModels[path];
            info.count--;
            if (info.count <= 0) LoadedModels.erase(path);
        }
    }

    void ModelMesh::loadModel(const std::string &path) {
        std::cout << "Loading model " << path << "\n";
        Assimp::Importer import;
        std::cout << "Importer initialized 1\n";

        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals | aiProcess_LimitBoneWeights);
        std::cout << "Scene loaded! \n";

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "Assimp Importer Error: " << import.GetErrorString() << "\n";
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene, aiMatrix4x4());
        Model::GlobalBoneMaps[path] = (Model::GlobalBoneMap){GetBoneInfoMap(), GetBoneCount()};

        import.FreeScene();
    }

    VaultRenderer::Mesh *ModelMesh::GetMeshValueByIndex(int index) {
        return &meshes[index];
    }

    void ModelMesh::processNode(aiNode *node, const aiScene *scene, const aiMatrix4x4 &matrix) {
        glm::mat4 transform = glm::mat4(1.0f);
        aiMatrix4x4 aiTransform = matrix * node->mTransformation;

        transform[0][0] = aiTransform.a1;
        transform[0][1] = aiTransform.b1;
        transform[0][2] = aiTransform.c1;
        transform[0][3] = aiTransform.d1;
        transform[1][0] = aiTransform.a2;
        transform[1][1] = aiTransform.b2;
        transform[1][2] = aiTransform.c2;
        transform[1][3] = aiTransform.d2;
        transform[2][0] = aiTransform.a3;
        transform[2][1] = aiTransform.b3;
        transform[2][2] = aiTransform.c3;
        transform[2][3] = aiTransform.d3;
        transform[3][0] = aiTransform.a4;
        transform[3][1] = aiTransform.b4;
        transform[3][2] = aiTransform.c4;
        transform[3][3] = aiTransform.d4;

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            auto &vaultMesh = processMesh(mesh, scene);
            vaultMesh.transform = transform;
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, aiTransform);
        }
    }

    VaultRenderer::Mesh &ModelMesh::processMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<VaultRenderer::Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<VaultRenderer::Texture> textures;

        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            VaultRenderer::Vertex vertex;
            SetVertexBoneDataToDefault(vertex);
            vertex.position.x = mesh->mVertices[i].x;
            vertex.position.y = mesh->mVertices[i].y;
            vertex.position.z = mesh->mVertices[i].z;

            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;

            if (mesh->mTextureCoords[0]) {
                vertex.texUV.x = mesh->mTextureCoords[0][i].x;
                vertex.texUV.y = mesh->mTextureCoords[0][i].y;
            } else {
                vertex.texUV = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        // size_t size = vertices.size();
        // // Open Binary file, to write out data
        // std::ofstream file("./raw_vector", std::ios::binary);
        // if (!file)
        //     std::cout << "Something went wrong" << std::endl;
        // // Store/Write the vector size
        // file.write(reinterpret_cast<const char *>(&size), sizeof(size));
        // // Store/Write the data of the vector out
        // file.write(reinterpret_cast<const char *>(vertices.data()), sizeof(vertices[0]) * size);
        // // Close binary file
        // file.close();

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        ExtractBoneWeightForVertices(vertices, mesh, scene);

        meshes.push_back(VaultRenderer::Mesh(vertices, indices, mesh->mName.C_Str()));

        aiString mot_name;
        aiReturn ret;
        aiMaterial *mot = scene->mMaterials[mesh->mMaterialIndex];

        ret = mot->Get(AI_MATKEY_NAME, mot_name);
        if (ret != AI_SUCCESS) {
            mot_name = "";
        }

        int diffuse_count = mot->GetTextureCount(aiTextureType_DIFFUSE);
        int specular_count = mot->GetTextureCount(aiTextureType_SPECULAR);
        int normal_count = mot->GetTextureCount(std::string(path).ends_with(".obj") ? aiTextureType_HEIGHT : aiTextureType_NORMALS);
        int normalc_count = mot->GetTextureCount(aiTextureType_NORMAL_CAMERA);
        int metallic = mot->GetTextureCount(aiTextureType_METALNESS);
        int roughness = mot->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
        int ao = mot->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
        aiString texture_name;

        if (diffuse_count > 0) {
            ret = mot->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_name);
            std::string sTexName = texture_name.C_Str();
            std::replace(sTexName.begin(), sTexName.end(), '\\', '/');
            std::cout << (directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str() << "\n";
            meshes.back().material.SetDiffuse((directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str());
        }
        if (specular_count > 0) {
            ret = mot->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), texture_name);
            std::string sTexName = texture_name.C_Str();
            std::replace(sTexName.begin(), sTexName.end(), '\\', '/');
            meshes.back().material.SetSpecular((directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str());
        }
        if (normal_count > 0) {
            ret = mot->Get(AI_MATKEY_TEXTURE(std::string(path).ends_with(".obj") ? aiTextureType_HEIGHT : aiTextureType_NORMALS, 0), texture_name);
            std::string sTexName = texture_name.C_Str();
            std::replace(sTexName.begin(), sTexName.end(), '\\', '/');
            // std::cout << (directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str() << " NORMALS\n";

            meshes.back().material.SetNormal((directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str());
        }
        if (metallic > 0) {
            ret = mot->Get(AI_MATKEY_TEXTURE(aiTextureType_METALNESS, 0), texture_name);
            std::string sTexName = texture_name.C_Str();
            std::replace(sTexName.begin(), sTexName.end(), '\\', '/');
            // std::cout << (directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str() << " NORMALS\n";

            meshes.back().material.SetMetallic((directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str());
        }
        if (roughness > 0) {
            ret = mot->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, 0), texture_name);
            std::string sTexName = texture_name.C_Str();
            std::replace(sTexName.begin(), sTexName.end(), '\\', '/');
            // std::cout << (directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str() << " NORMALS\n";

            meshes.back().material.SetRoughness((directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str());
        }
        if (ao > 0) {
            ret = mot->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT_OCCLUSION, 0), texture_name);
            std::string sTexName = texture_name.C_Str();
            std::replace(sTexName.begin(), sTexName.end(), '\\', '/');
            // std::cout << (directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str() << " NORMALS\n";

            meshes.back().material.SetAO((directory + "/textures/" + std::filesystem::path(sTexName).filename().string()).c_str());
        }

        if (!std::filesystem::exists("./assets/mesh_materials")) std::filesystem::create_directory("./assets/mesh_materials");
        const std::string material_name = "./assets/mesh_materials/" + uuid::generate_uuid_v4() + "_" + meshes.back().name + ".material";
        Serializer::SerializeMaterial(material_name, meshes.back().material);
        meshes.back().material.filePath = material_name;
        std::cout << "Created " << material_name << "\n";

        return meshes.back();
    }

    std::vector<VaultRenderer::Texture> ModelMesh::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName) {
    }

    void ModelMesh::SetVertexBoneDataToDefault(VaultRenderer::Vertex &vertex) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }

    void ModelMesh::SetVertexBoneData(VaultRenderer::Vertex &vertex, int boneID, float weight) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if (vertex.m_BoneIDs[i] < 0) {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }

    void ModelMesh::ExtractBoneWeightForVertices(std::vector<VaultRenderer::Vertex> &vertices, aiMesh *mesh, const aiScene *scene) {
        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
                VaultRenderer::BoneInfo newBoneInfo;
                newBoneInfo.id = m_BoneCounter;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);

                m_BoneInfoMap[boneName] = newBoneInfo;
                boneID = m_BoneCounter;
                m_BoneCounter++;
            } else {
                boneID = m_BoneInfoMap[boneName].id;
            }

            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;

                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }
} // namespace Engine