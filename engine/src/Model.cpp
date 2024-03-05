#include <Engine/Model.hpp>
#include <Engine/Components/MeshRenderer.hpp>
#include <iostream>
#include <fstream>

namespace Engine {
    DLL_API std::map<std::string, Model::GlobalBoneMap> Model::GlobalBoneMaps;

    Model::Model(const std::string &path) : path(path) {
        parent = GameObject::New("Model");
        loadModel(path);
    }

    Model::Model(const char *path) : path(path) {
        parent = GameObject::New("Model");
        loadModel(path);
    }

    void Model::loadModel(const std::string &path) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FixInfacingNormals | aiProcess_LimitBoneWeights);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "Assimp Importer Error: " << import.GetErrorString() << "\n";
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
        GlobalBoneMaps[path] = (GlobalBoneMap){GetBoneInfoMap(), GetBoneCount()};
    }

    void Model::processNode(aiNode *node, const aiScene *scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    void Model::processMesh(aiMesh *mesh, const aiScene *scene) {
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
        // std::ofstream file("./raw_vector", std::ios::binary);
        // file.write(reinterpret_cast<const char *>(&size), sizeof(size));
        // file.write(reinterpret_cast<const char *>(vertices.data()), sizeof(vertices[0]) * size);
        // file.close();

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        ExtractBoneWeightForVertices(vertices, mesh, scene);

        using namespace Engine::Components;
        auto &pChild = GameObject::New(mesh->mName.C_Str());
        pChild->parent = parent->ID;
        std::cout << pChild->name << "\n";
        pChild->AddComponent<MeshRenderer>();

        auto &meshRenderer = pChild->GetComponent<MeshRenderer>();
        meshRenderer.SetCustomMeshType(vertices, indices);
        meshRenderer.mesh_index = index;
        meshRenderer.mesh_path = path;
        index++;
    }

    std::vector<VaultRenderer::Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName) {
    }

    void Model::SetVertexBoneDataToDefault(VaultRenderer::Vertex &vertex) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }

    void Model::SetVertexBoneData(VaultRenderer::Vertex &vertex, int boneID, float weight) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if (vertex.m_BoneIDs[i] < 0) {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }

    void Model::ExtractBoneWeightForVertices(std::vector<VaultRenderer::Vertex> &vertices, aiMesh *mesh, const aiScene *scene) {
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

    // -- BONE START
    Bone::Bone(const std::string &name, int ID, const aiNodeAnim *channel) : name(name), id(ID), local_transform(1.0f) {
        numPos = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < numPos; ++positionIndex) {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyframePos data;
            data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
            data.timeStamp = timeStamp;
            positions.push_back(data);
        }

        numRots = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < numRots; ++rotationIndex) {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyframeRot data;
            data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
            data.timeStamp = timeStamp;
            rotations.push_back(data);
        }

        numScales = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < numScales; ++keyIndex) {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyframeScale data;
            data.scale = AssimpGLMHelpers::GetGLMVec(scale);
            data.timeStamp = timeStamp;
            scales.push_back(data);
        }
    }

    int Bone::GetPositionIndex(float anim_time) {
        for (int index = 0; index < numPos - 1; ++index) {
            if (anim_time < positions[index + 1].timeStamp)
                return index;
        }
        return 0;
    }

    int Bone::GetRotationIndex(float anim_time) {
        for (int index = 0; index < numRots - 1; ++index) {
            if (anim_time < rotations[index + 1].timeStamp)
                return index;
        }
        return 0;
    }

    int Bone::GetScaleIndex(float anim_time) {
        for (int index = 0; index < numScales - 1; ++index) {
            if (anim_time < scales[index + 1].timeStamp)
                return index;
        }
        return 0;
    }

    float Bone::scale_factor(float last, float next, float anim_time) {
        float factor = 0.0f;
        float midway = anim_time - last;
        float diff = next - last;
        factor = midway / diff;
        return factor;
    }

    glm::mat4 Bone::InterPos(float anim_time) {
        if (numPos == 1)
            return glm::translate(glm::mat4(1.0f), positions[0].position);

        int pos0 = GetPositionIndex(anim_time);
        int pos1 = pos0 + 1;

        float scaleFactor = scale_factor(positions[pos0].timeStamp, positions[pos1].timeStamp, anim_time);
        glm::vec3 final = glm::mix(positions[pos0].position, positions[pos1].position, scaleFactor);

        return glm::translate(glm::mat4(1.0f), final);
    }

    glm::mat4 Bone::InterRot(float anim_time) {
        if (numRots == 1) {
            return glm::toMat4(glm::normalize(rotations[0].orientation));
        }

        int rot0 = GetRotationIndex(anim_time);
        int rot1 = rot0 + 1;

        float factor = scale_factor(rotations[rot0].timeStamp, rotations[rot1].timeStamp, anim_time);
        glm::quat final = glm::slerp(rotations[rot0].orientation, rotations[rot1].orientation, factor);

        final = glm::normalize(final);
        return glm::toMat4(final);
    }

    glm::mat4 Bone::InterScal(float anim_time) {
        if (numScales == 1)
            return glm::scale(glm::mat4(1.0f), scales[0].scale);

        int scale0 = GetScaleIndex(anim_time);
        int scale1 = scale0 + 1;

        float factor = scale_factor(scales[scale0].timeStamp, scales[scale1].timeStamp, anim_time);
        glm::vec3 final = glm::mix(scales[scale0].scale, scales[scale1].scale, factor);

        return glm::scale(glm::mat4(1.0f), final);
    }

    void Bone::SetLocalTransform() {
        local_transform =
            glm::translate(glm::mat4(1.0f), m_pos) *
            glm::toMat4(glm::quat(m_rot)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(m_scal.x * 0.5, m_scal.y * 0.5, m_scal.z * 0.5));
    }
    // -- BONE END

    // -- ANIMATION BEGIN
    void Animation::ReadMissingBones(const aiAnimation *animation) {
        int size = animation->mNumChannels;

        auto &modelBoneMap = GetBoneInfoMap();
        int &count = GetBoneCount();

        for (int i = 0; i < size; i++) {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (modelBoneMap.find(boneName) == modelBoneMap.end()) {
                modelBoneMap[boneName].id = count;
                count++;
            }

            bones.push_back(Bone(channel->mNodeName.data, modelBoneMap[channel->mNodeName.data].id, channel));
        }
    }

    void Animation::ReadHeirarchyData(AssimpNodeData &dest, const aiNode *src) {
        dest.name = src->mName.data;
        dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++) {
            AssimpNodeData newData;
            ReadHeirarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }

    Bone *Animation::FindBone(const std::string &name) {
        auto iter = std::find_if(bones.begin(), bones.end(), [&](const Bone &Bone) { return Bone.GetName() == name; });
        return iter == bones.end() ? nullptr : &(*iter);
    }

    Animation::Animation(const std::string &animationPath, Model::GlobalBoneMap &bone_map) : bonemap(bone_map) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[0];
        duration = animation->mDuration;
        ticks_per_sec = animation->mTicksPerSecond;
        ReadHeirarchyData(root_node, scene->mRootNode);
        ReadMissingBones(animation);
    }
    Animation::~Animation() {}
    // -- ANIMATION END

    // -- ANIMATOR BEGIN
    Animator::Animator(Animation *anim) {
        curr_time = 0.0;
        curr_anim = anim;

        finalBoneMatrices.reserve(100);

        for (int i = 0; i < 100; i++) {
            finalBoneMatrices.push_back(glm::mat4(1.0f));
        }
    }

    void Animator::UpdateAnimation(float dt) {
        timestep = dt;
        if (curr_anim) {
            curr_time += curr_anim->GetTicksPerSecond() * dt;
            curr_time = fmod(curr_time, curr_anim->GetDuration());
            CalculateBoneTransform(&curr_anim->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void Animator::UpdateTransforms() {
        LocalTransformCalculateBoneTransform(&curr_anim->GetRootNode(), glm::mat4(1.0f));
    }

    void Animator::PlayAnimation(Animation *anim) {
        curr_anim = anim;
        curr_time = 0.0f;
    }

    void Animator::CalculateBoneTransform(const AssimpNodeData *node, const glm::mat4 &parent) {
        const std::string &nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone *Bone = curr_anim->FindBone(nodeName);

        if (Bone) {
            Bone->Update(curr_time);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parent * nodeTransform;

        auto &boneInfoMap = curr_anim->GetIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            finalBoneMatrices[index] = globalTransformation * boneInfoMap.at(nodeName).offset;
        }

        for (int i = 0; i < node->childrenCount; i++) {
            CalculateBoneTransform(&node->children[i], globalTransformation);
        }
    }

    void Animator::LocalTransformCalculateBoneTransform(const AssimpNodeData *node, const glm::mat4 &parent) {
        const std::string &nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone *Bone = curr_anim->FindBone(nodeName);

        if (Bone) {
            Bone->SetLocalTransform();
            nodeTransform = Bone->local_transform;
        }

        glm::mat4 globalTransformation = parent * nodeTransform;

        auto &boneInfoMap = curr_anim->GetIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            finalBoneMatrices[index] = globalTransformation * boneInfoMap.at(nodeName).offset;
        }

        for (int i = 0; i < node->childrenCount; i++) {
            LocalTransformCalculateBoneTransform(&node->children[i], globalTransformation);
        }
    }

    std::vector<glm::mat4> Animator::GetFinalBoneMatrices() {
        return finalBoneMatrices;
    }

} // namespace Engine