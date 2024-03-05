#include <Engine/Model.hpp>
#include <Engine/Components/MeshRenderer.hpp>
#include <iostream>
#include <fstream>

namespace Engine {
    ModelMesh::ModelMesh(const std::string &path) : path(path) {
        loadModel(path);
    }

    ModelMesh::ModelMesh(const char *path) : path(path) {
        loadModel(path);
    }

    void ModelMesh::loadModel(const std::string &path) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FixInfacingNormals | aiProcess_LimitBoneWeights);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "Assimp Importer Error: " << import.GetErrorString() << "\n";
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
        Model::GlobalBoneMaps[path] = (Model::GlobalBoneMap){GetBoneInfoMap(), GetBoneCount()};
    }

    VaultRenderer::Mesh *ModelMesh::GetMeshValueByIndex(int index) {
        return &meshes[index];
    }

    void ModelMesh::processNode(aiNode *node, const aiScene *scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    void ModelMesh::processMesh(aiMesh *mesh, const aiScene *scene) {
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
                vertex.texUV.x = mesh->mTextureCoords[0][i].y;
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

        meshes.push_back(VaultRenderer::Mesh(vertices, indices));

        aiString mot_name;
        aiReturn ret;
        aiMaterial *mot = scene->mMaterials[mesh->mMaterialIndex];

        ret = mot->Get(AI_MATKEY_NAME, mot_name);
        if (ret != AI_SUCCESS) {
            mot_name = "";
        }

        int diffuse_count = mot->GetTextureCount(aiTextureType_DIFFUSE);
        int specular_count = mot->GetTextureCount(aiTextureType_SPECULAR);
        int normal_count = mot->GetTextureCount(aiTextureType_NORMALS);
        aiString texture_name;

        if (diffuse_count > 0) {
            ret = mot->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_name);
            std::cout << (directory + "/" + texture_name.C_Str()).c_str() << "\n";
            meshes.back().material.SetDiffuse((directory + "/" + texture_name.C_Str()).c_str());
        }
        if (specular_count > 0) {
            ret = mot->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), texture_name);
            meshes.back().material.SetSpecular((directory + "/" + texture_name.C_Str()).c_str());
        }
        if (normal_count > 0) {
            ret = mot->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), texture_name);
            meshes.back().material.SetNormal((directory + "/" + texture_name.C_Str()).c_str());
        }
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