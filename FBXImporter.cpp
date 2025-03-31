#include "Renderer/Mesh.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/gtx/quaternion.hpp"
#include <Engine/FBXImporter.hpp>
#include <Engine/DecomposeTransform.hpp>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>

glm::mat4 NormalizeScale(const glm::mat4 &transform) {
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec3 translation;
    glm::quat rotation;
    glm::vec4 perspective;

    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    // Normalize the scale
    glm::vec3 normalizedScale = glm::vec3(1.0f);

    // Recompose matrix without scale
    glm::mat4 normalizedMatrix = glm::mat4_cast(rotation);
    normalizedMatrix[3] = glm::vec4(translation, 1.0f);
    return normalizedMatrix;
}

namespace std {
    template <>
    struct hash<VaultRenderer::Vertex> {
        std::size_t operator()(const VaultRenderer::Vertex &vertex) const {
            std::size_t h1 = std::hash<float>()(vertex.position.x);
            std::size_t h2 = std::hash<float>()(vertex.position.y);
            std::size_t h3 = std::hash<float>()(vertex.position.z);
            std::size_t h4 = std::hash<float>()(vertex.normal.x);
            std::size_t h5 = std::hash<float>()(vertex.normal.y);
            std::size_t h6 = std::hash<float>()(vertex.normal.z);
            std::size_t h10 = std::hash<float>()(vertex.texUV.x);
            std::size_t h11 = std::hash<float>()(vertex.texUV.y);
            return h1 ^ h2 ^ h3 ^ h4 ^ h5 ^ h6 ^ h10 ^ h11;
        }
    };
} // namespace std
namespace VaultRenderer {
    FBXLoadResult FBXImporter::ImportModel(std::string filePath) {
        FBXLoadResult result;

        std::ifstream file(filePath, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            result.isSuccessful = false;
            result.message = "Can't open the file path specified " + filePath;
            return result;
        }

        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> fileData(fileSize);

        if (!file.read(fileData.data(), fileSize)) {
            result.isSuccessful = false;
            result.message = "Can't read file data, maybe it's corrupted.";
            return result;
        }

        ofbx::LoadFlags flags = ofbx::LoadFlags::IGNORE_CAMERAS;

        auto scene = ofbx::load((ofbx::u8 *)fileData.data(), fileSize, (ofbx::u16)flags);
        m_Scene = scene;

        // process the scene.
        ProcessScene(scene);

        for (auto &mesh : m_Meshes) {
            result.meshes.push_back(mesh.mesh);
        }

        result.animations = m_Animations;
        result.name = m_Meshes[0].name;
        return result;
    }

    void FBXImporter::ProcessScene(ofbx::IScene *scene) {
        auto rootObject = scene->getRoot();

        GatherBones(*rootObject);
        SortBones();

        GatherMeshes(*rootObject);
        GatherAnimations();

        ProcessBones();

        WriteSkeletons();
    }

    void FBXImporter::GatherMeshes(const ofbx::Object &parent) {
        ofbx::Object::Type objectType = parent.getType();
        // process current object according to type
        switch (objectType) {
        case ofbx::Object::Type::MESH: {
            bool has_valid_skins = false;
            auto skins = GetSkinsForMesh((ofbx::Mesh *)&parent, has_valid_skins);
            auto meshResult = ProcessMeshNode((ofbx::Mesh *)&parent, skins, has_valid_skins);
            ImportedMesh newMesh;
            newMesh.mesh = meshResult;
            newMesh.obj = &parent;
            newMesh.name = parent.name;
            m_Meshes.push_back(newMesh);
            break;
        }
        default:
            break;
        }

        // process next objects
        int i = 0;
        while (ofbx::Object *child = parent.resolveObjectLink(i)) {
            GatherMeshes(*child);
            ++i;
        }
    }

    void FBXImporter::GatherBones(const ofbx::Object &parent) {
        ofbx::Object::Type objectType = parent.getType();
        switch (objectType) {
        case ofbx::Object::Type::SKIN: {
            ofbx::Skin *skin = (ofbx::Skin *)&parent;
            int cluster_count = skin->getClusterCount();
            for (size_t i = 0; i < cluster_count; i++) {
                auto cluster = skin->getCluster(i);
                InsertBone(cluster->getLink());
            }
            break;
        }
        }

        // process next objects
        int i = 0;
        while (ofbx::Object *child = parent.resolveObjectLink(i)) {
            GatherBones(*child);
            ++i;
        }
    }

    void FBXImporter::ProcessBones() {
        int bone_index = 0;
        for (auto &bone : m_Bones) {
            glm::mat4 bindPoseMatrix = GetBindPoseMatrix(bone);
            NormalizeBoneScale(bindPoseMatrix, true);

            bone.GlobalBindPoseMatrix = bindPoseMatrix;
            bone.GlobalInverseBindPoseMatrix = glm::inverse(bindPoseMatrix);

            for (auto &mesh : m_Meshes) {
                // if (mesh.obj == boneMesh)
                //{
                //	LinkMeshToBones(mesh, bone_index);
                // }
            }
            bone_index++;
        }
    }

    void FBXImporter::LinkMeshToBones(ImportedMesh &mesh, int32_t bone_id) {
        auto &parentBone = m_Bones[bone_id];

        int32_t bone_index = 0;
        for (auto &bone : m_Bones) {
            if (bone.ParentID == parentBone.ID) {
                LinkMeshToBones(mesh, bone_index);
            }

            if (bone.ParentID == 0) {
                mesh.first_bone_id = bone_index;
                break;
            }
            bone_index++;
        }
    }

    std::shared_ptr<Mesh> FBXImporter::ProcessMeshNode(const ofbx::Mesh *mesh, std::vector<ImportedSkin> skins, bool hasValidSkins) {
        const ofbx::Geometry *geometry = mesh->getGeometry();

        auto &geom = geometry->getGeometryData();

        ofbx::Vec3Attributes positions = geom.getPositions();
        ofbx::Vec3Attributes normals = geom.getNormals();
        ofbx::Vec3Attributes tangents = geom.getTangents();
        ofbx::Vec2Attributes uvs = geom.getUVs();

        std::vector<Vertex> vertexBuffer;
        std::vector<uint16_t> indexBuffer;
        std::unordered_map<Vertex, uint32_t> uniqueVertices;

        for (int partition_idx = 0; partition_idx < geom.getPartitionCount(); ++partition_idx) {
            const ofbx::GeometryPartition &partition = geom.getPartition(partition_idx);
            std::vector<int> tri_indices(partition.max_polygon_triangles * 3);

            for (int polygon_idx = 0; polygon_idx < partition.polygon_count; ++polygon_idx) {
                const ofbx::GeometryPartition::Polygon &polygon = partition.polygons[polygon_idx];
                uint32_t tri_count = ofbx::triangulate(geom, polygon, tri_indices.data());

                for (uint32_t i = 0; i < tri_count; ++i) {
                    int idx = tri_indices[i];

                    Vertex vertex;

                    ofbx::Vec3 position = positions.get(idx);
                    ofbx::Vec3 normal = normals.get(idx);
                    ofbx::Vec2 UV = uvs.get(idx);

                    glm::mat4 meshGlobalTransform = OFBXMatrixToVoidMatrix(mesh->getGlobalTransform());
                    float scene_scale = m_Scene->getGlobalSettings()->UnitScaleFactor * 0.01f;
                    glm::vec4 transformed_vertex = meshGlobalTransform * glm::vec4(position.x, position.y, position.z, 1.0f);

                    transformed_vertex *= scene_scale;
                    vertex.position.x = transformed_vertex.x;
                    vertex.position.y = transformed_vertex.y;
                    vertex.position.z = transformed_vertex.z;

                    UV.y = 1.0f - UV.y;
                    vertex.texUV.x = UV.x;
                    vertex.texUV.y = UV.y;

                    vertex.normal.x = normal.x;
                    vertex.normal.y = normal.y;
                    vertex.normal.z = normal.z;

                    if (positions.indices && hasValidSkins) {
                        vertex.m_BoneIDs[0] = skins[positions.indices[tri_indices[i]]].joints[0];
                        vertex.m_BoneIDs[1] = skins[positions.indices[tri_indices[i]]].joints[1];
                        vertex.m_BoneIDs[2] = skins[positions.indices[tri_indices[i]]].joints[2];
                        vertex.m_BoneIDs[3] = skins[positions.indices[tri_indices[i]]].joints[3];

                        vertex.m_Weights[1] = skins[positions.indices[tri_indices[i]]].weights[0];
                        vertex.m_Weights[2] = skins[positions.indices[tri_indices[i]]].weights[1];
                        vertex.m_Weights[3] = skins[positions.indices[tri_indices[i]]].weights[2];
                        vertex.m_Weights[4] = skins[positions.indices[tri_indices[i]]].weights[3];
                    }
                    // Check if vertex already exists
                    if (uniqueVertices.count(vertex) == 0) {
                        // Add unique vertex and store its index
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertexBuffer.size());
                        vertexBuffer.push_back(vertex);
                    }

                    // Add the index of the vertex to the index buffer
                    indexBuffer.push_back(uniqueVertices[vertex]);
                }
            }
        }

        ComputeTangentsAndBitangents(vertexBuffer, indexBuffer);

        std::shared_ptr<Mesh> def;

        // uint32_t material_count = mesh->getMaterialCount();
        // for (uint32_t i = 0; i < material_count; i++) {
        //     const ofbx::Material *material = mesh->getMaterial(i);

        //     def->raw_diffuse_texture = GetTextureDataFromMaterial(material, ofbx::Texture::TextureType::DIFFUSE, def->raw_diffuse_texture_size);

        //     def->raw_metallic_roughness_texture = GetTextureDataFromMaterial(material, ofbx::Texture::TextureType::REFLECTION, def->raw_metallic_roughness_texture_size);

        //     def->raw_normal_texture = GetTextureDataFromMaterial(material, ofbx::Texture::TextureType::NORMAL, def->raw_normal_texture_size);
        // }
        return def;
    }

    glm::mat4 FBXImporter::OFBXMatrixToVoidMatrix(ofbx::DMatrix matrix) {
        glm::mat4 res;              // glm::mat4 is column-major by default
        const double *m = matrix.m; // Access the raw data of ofbx::DMatrix

        // Copy each element, casting from double to float
        for (int i = 0; i < 16; ++i) {
            glm::value_ptr(res)[i] = static_cast<float>(m[i]);
        }

        return res;
    }

    void FBXImporter::WriteSkeletons() {
        for (auto &mesh : m_Meshes) {
            // // TODO : figure out a way to deal with more than 1 fucking skinned mesh.

            // std::shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();

            // skeleton->m_Bones.resize(m_Bones.size());

            // for (auto &bone : m_Bones) {
            //     std::vector<uint32_t> childBoneIndices = GetChildBoneIndices(bone);

            //     std::shared_ptr<Skeleton::Bone> vBone = std::make_shared<Skeleton::Bone>();

            //     vBone->m_ChildrenIndices = childBoneIndices;
            //     vBone->m_BoneName = bone.Name;
            //     if (bone.parent_index == -999) {
            //         vBone->m_ParentIndex = VMDL_BONE_NO_PARENT;
            //     } else {
            //         vBone->m_ParentIndex = bone.parent_index;
            //     }
            //     vBone->m_Index = bone.index;
            //     vBone->m_InverseBindPoseMatrix = bone.GlobalInverseBindPoseMatrix;
            //     vBone->m_BindPoseMatrix = bone.GlobalBindPoseMatrix;

            //     // when first importing a model, the global bone transform is in bind pose as well.
            //     vBone->m_BoneTransform = bone.GlobalBindPoseMatrix;

            //     skeleton->InsertBone(vBone, bone.index);
            // }

            // for (auto bone : skeleton->m_Bones) {
            //     if (bone->m_ParentIndex == VMDL_BONE_NO_PARENT) {
            //         bone->m_LocalBoneTransform = bone->m_BoneTransform;
            //     } else {
            //         auto parent_bone = skeleton->GetBone(bone->m_ParentIndex);
            //         bone->m_LocalBoneTransform = glm::inverse(parent_bone->m_BoneTransform) * bone->m_BoneTransform;
            //     }
            // }

            // skeleton->UpdateBoneTransforms();
            // mesh.mesh->m_Skeleton = skeleton;
        }
    }

    std::vector<uint32_t> FBXImporter::GetChildBoneIndices(ImportedBone &parent) {
        std::vector<uint32_t> result;

        for (auto &bone : m_Bones) {
            if (bone.parent_index == parent.index) {
                result.push_back(bone.index);
            }
        }

        return result;
    }

    glm::mat4 FBXImporter::FixOrientation(glm::mat4 m) {
        const ofbx::GlobalSettings *settings = m_Scene->getGlobalSettings();
        ofbx::UpVector up = settings->UpAxis;

        switch (up) {
        case ofbx::UpVector_AxisX:
            break;
        case ofbx::UpVector_AxisY:
            return m;
        case ofbx::UpVector_AxisZ: {
            glm::mat4 mtx = glm::mat4(
                glm::vec4(1, 0, 0, 0),
                glm::vec4(0, 0, -1, 0),
                glm::vec4(0, 1, 0, 0),
                glm::vec4(0, 0, 0, 1));
            return mtx * m;
            break;
        }
        default:
            break;
        }

        return m;
    }

    void FBXImporter::SortBones() {
        for (auto &bone : m_Bones) {
            const ofbx::Object *node = (const ofbx::Object *)bone.ID;
            bone.ParentID = node->getParent() ? uint64_t(node->getParent()) : 0;
        }

        const int count = m_Bones.size();
        uint32_t first_nonroot = 0;

        for (int i = 0; i < count; ++i) {
            if (m_Bones[i].ParentID == 0) {
                auto temp = m_Bones[i];

                m_Bones[i] = m_Bones[first_nonroot];
                m_Bones[i].index = i;
                m_Bones[i].parent_index = 0;

                m_Bones[first_nonroot] = temp;
                m_Bones[first_nonroot].index = first_nonroot;
                m_Bones[first_nonroot].parent_index = 0;
                ++first_nonroot;
            }
        }

        for (int i = 0; i < count; ++i) {
            for (int j = i + 1; j < count; ++j) {
                if (m_Bones[i].ParentID == m_Bones[j].ID) {

                    auto bone = m_Bones[j];
                    bone.index = i;
                    bone.parent_index = j;
                    m_Bones.erase(m_Bones.begin() + j);
                    m_Bones.insert(m_Bones.begin() + i, bone);
                    --i;
                    break;
                }
            }
        }

        // set actual bone indices.
        for (int i = 0; i < count; ++i) {
            m_Bones[i].index = i;
        }

        // set bone parent indices.
        for (int i = 0; i < count; ++i) {
            for (int j = 0; j < count; ++j) {
                if (m_Bones[i].ParentID == m_Bones[j].ID) {
                    m_Bones[i].parent_index = j;
                }
            }
        }
    }

    void FBXImporter::InsertBone(const ofbx::Object *node) {
        if (!node) {
            return;
        }

        // ignore if bone ID already exists
        if (std::find_if(m_Bones.begin(), m_Bones.end(), [node](ImportedBone b) { return b.ID == uint64_t(node); }) != m_Bones.end()) {
            return;
        }

        ofbx::Object *objectParent = node->getParent();
        if (objectParent->getType() != ofbx::Object::Type::ROOT) {
            InsertBone(objectParent);
        }

        ImportedBone b;
        b.ID = uint64_t(node);

        b.Name = node->name;

        m_Bones.push_back(b);
    }

    static bool isConstCurve(const ofbx::AnimationCurve *curve) {
        if (!curve) return true;
        if (curve->getKeyCount() <= 1) return true;
        const float *values = curve->getKeyValue();
        if (curve->getKeyCount() == 2 && fabsf(values[1] - values[0]) < 1e-6) return true;
        return false;
    }

    void FBXImporter::GatherAnimations() {
        int anim_count = m_Scene->getAnimationStackCount();
        for (int i = 0; i < anim_count; ++i) {

            ImportedAnimation anim;
            anim.animation_id = i;

            const ofbx::AnimationStack *fbx_anim = (const ofbx::AnimationStack *)m_Scene->getAnimationStack(i);
            {
                const ofbx::TakeInfo *take_info = m_Scene->getTakeInfo(fbx_anim->name);
                if (take_info) {
                    if (take_info->name.begin != take_info->name.end) {

                        char animation_name[128];
                        take_info->name.toString(animation_name);
                        anim.name = std::string(animation_name);
                    }
                    if (anim.name.empty() && take_info->filename.begin != take_info->filename.end) {
                    }
                    if (anim.name.empty()) anim.name = "anim";
                } else {
                    anim.name = "";
                }
            }

            const ofbx::AnimationLayer *anim_layer = fbx_anim->getLayer(0);

            {
                anim.fps = m_Scene->getSceneFrameRate();
                const ofbx::TakeInfo *take_info = m_Scene->getTakeInfo(fbx_anim->name);

                if (!take_info && anim.name.starts_with("AnimStack::")) {
                    take_info = m_Scene->getTakeInfo(fbx_anim->name + 11);
                }

                if (take_info) {
                    anim.duration = take_info->local_time_to - take_info->local_time_from;
                } else if (m_Scene->getGlobalSettings()) {
                    anim.duration = m_Scene->getGlobalSettings()->TimeSpanStop;
                } else {
                    std::cout << "ERROR :Unsupported animation";
                    continue;
                }

                anim.tracks.resize(m_Bones.size());
                GatherKeyframes(anim);

                auto first_bone_tracks = anim.tracks[0];
                m_Animations.push_back(anim);
            }
        }
    }

    static int64_t sampleToFBXTime(uint32_t sample, float fps) {
        return ofbx::secondsToFbxTime(sample / fps);
    }

    static float EvalCurve(int64_t time, const ofbx::AnimationCurve &curve) {
        const int64_t *times = curve.getKeyTime();
        const float *values = curve.getKeyValue();
        const int count = curve.getKeyCount();

        // V_ASSERT(count > 0, "");

        time = std::clamp(time, times[0], times[count - 1]);

        for (int i = 0; i < count; ++i) {
            if (time == times[i]) {
                return values[i];
            }
            if (time < times[i]) {
                // V_ASSERT(i > 0, "");
                // V_ASSERT(time > times[i - 1], "");
                const float t = float((time - times[i - 1]) / double(times[i] - times[i - 1]));
                return values[i - 1] * (1 - t) + values[i] * t;
            }
        }
        // V_ASSERT(false, "");
        return 0.f;
    };

    void FBXImporter::GatherKeyframes(ImportedAnimation &animation) {
        const ofbx::AnimationStack *fbx_anim = (const ofbx::AnimationStack *)m_Scene->getAnimationStack(animation.animation_id);
        const ofbx::AnimationLayer *layer = fbx_anim->getLayer(0);

        uint32_t bone_idx = 0;
        for (const auto &bone : m_Bones) {

            ofbx::Object *boneObj = (ofbx::Object *)uint64_t(bone.ID);

            const ofbx::AnimationCurveNode *translation_node = layer->getCurveNode(*boneObj, "Lcl Translation");
            const ofbx::AnimationCurveNode *rotation_node = layer->getCurveNode(*boneObj, "Lcl Rotation");

            std::vector<ImportedAnimationBoneKeyFrame> keys;

            auto samples_count = uint32_t(animation.duration * animation.fps + 0.5f) + 1;
            keys.resize(samples_count);

            if (!translation_node && !rotation_node) {
                continue;
            }

            auto fill_pos = [&](uint32_t idx, const ofbx::AnimationCurve *curve) {
                if (!curve) {
                    const ofbx::DVec3 lcl_pos = boneObj->getLocalTranslation();
                    for (auto &k : keys) {
                        k.position[idx] = float((&lcl_pos.x)[idx]);
                    }
                    return;
                }

                for (uint32_t f = 0; f < samples_count; ++f) {
                    auto &k = keys[f];
                    k.position[idx] = EvalCurve(sampleToFBXTime(f, animation.fps), *curve);
                }
            };

            auto fill_rot = [&](uint32_t idx, const ofbx::AnimationCurve *curve) {
                if (!curve) {
                    const ofbx::DVec3 lcl_rot = boneObj->getLocalRotation();
                    for (auto &k : keys) {
                        k.rotation[idx] = float((&lcl_rot.x)[idx]);
                    }
                    return;
                }
                for (uint32_t f = 0; f < samples_count; ++f) {
                    auto &k = keys[f];
                    k.rotation[idx] = EvalCurve(sampleToFBXTime(f, animation.fps), *curve);
                }
            };

            fill_pos(0, translation_node ? translation_node->getCurve(0) : nullptr);
            fill_pos(1, translation_node ? translation_node->getCurve(1) : nullptr);
            fill_pos(2, translation_node ? translation_node->getCurve(2) : nullptr);

            fill_rot(0, rotation_node ? rotation_node->getCurve(0) : nullptr);
            fill_rot(1, rotation_node ? rotation_node->getCurve(1) : nullptr);
            fill_rot(2, rotation_node ? rotation_node->getCurve(2) : nullptr);

            std::vector<ImportedAnimationFrameTransform> transforms;

            for (auto &key : keys) {
                const ofbx::DMatrix mtx = boneObj->evalLocal({key.position.x, key.position.y, key.position.z}, {key.rotation.x, key.rotation.y, key.rotation.z});

                glm::mat4 frameTransform = OFBXMatrixToVoidMatrix(mtx);
                NormalizeBoneScale(frameTransform, false);

                ImportedAnimationFrameTransform frame;
                frame.transform = frameTransform;

                transforms.push_back(frame);
            }
            animation.tracks[bone_idx] = transforms;

            bone_idx++;
        }
    }

    std::vector<ImportedSkin> FBXImporter::GetSkinsForMesh(const ofbx::Mesh *mesh, bool &hasValidSkins) {
        const ofbx::Skin *fbx_skin = mesh->getSkin();
        const ofbx::GeometryData &geom = mesh->getGeometryData();

        std::vector<ImportedSkin> skinning;
        skinning.resize(geom.getPositions().values_count);

        memset(&skinning[0], 0, skinning.size() * sizeof(skinning[0]));

        if (!fbx_skin) {
            for (ImportedSkin &skin : skinning) {
                skin.count = 1;
                skin.weights[0] = 1;
                skin.weights[1] = skin.weights[2] = skin.weights[3] = 0;
                skin.joints[0] = skin.joints[1] = skin.joints[2] = skin.joints[3] = 0;
            }

            hasValidSkins = false;
            return skinning;
        }

        for (int i = 0, c = fbx_skin->getClusterCount(); i < c; ++i) {
            const ofbx::Cluster *cluster = fbx_skin->getCluster(i);
            if (cluster->getIndicesCount() == 0) continue;
            if (!cluster->getLink()) continue;

            for (int32_t i = 0; i < m_Bones.size(); i++) {
                auto &bone = m_Bones[i];
                uint64_t cluster_id = uint64_t(cluster->getLink());
                if (bone.ID == cluster_id) {
                    int32_t joint = i;

                    const int *cp_indices = cluster->getIndices();
                    const double *weights = cluster->getWeights();

                    for (int j = 0; j < cluster->getIndicesCount(); ++j) {
                        int idx = cp_indices[j];
                        float weight = (float)weights[j];
                        ImportedSkin &s = skinning[idx];

                        // some weird fuckery I got from openfbx
                        if (s.count < 4) {
                            s.weights[s.count] = weight;
                            s.joints[s.count] = joint;
                            ++s.count;
                        } else {
                            int min = 0;
                            for (int m = 1; m < 4; ++m) {
                                if (s.weights[m] < s.weights[min]) min = m;
                            }

                            if (s.weights[min] < weight) {
                                s.weights[min] = weight;
                                s.joints[min] = joint;
                            }
                        }
                    }
                }
            }
        }

        for (ImportedSkin &s : skinning) {
            float sum = 0;
            for (float w : s.weights)
                sum += w;
            if (sum == 0) {
                s.weights[0] = 1;
                s.weights[1] = s.weights[2] = s.weights[3] = 0;
                s.joints[0] = s.joints[1] = s.joints[2] = s.joints[3] = 0;
            } else {
                for (float &w : s.weights)
                    w /= sum;
            }
        }

        hasValidSkins = true;
        return skinning;
    }

    const ofbx::Mesh *FBXImporter::GetAnyMeshFromBone(const ofbx::Object *node, int32_t bone_id) {
        for (const auto &geom : m_Meshes) {
            if (geom.first_bone_id == bone_id) {
                return (ofbx::Mesh *)geom.obj;
            }

            auto fbx_geom = (ofbx::Mesh *)geom.obj;

            auto *skin = fbx_geom->getSkin();
            if (!skin) continue;

            for (int j = 0, c = skin->getClusterCount(); j < c; ++j) {
                if (skin->getCluster(j)->getLink() == node) return fbx_geom;
            }
        }
        return nullptr;
    }

    void FBXImporter::NormalizeBoneScale(glm::mat4 &matrix, bool applySceneScale) {
        glm::mat4 normalizedBindPose = NormalizeScale(matrix);

        glm::vec3 position, scale, rotation;
        DecomposeTransform(normalizedBindPose, position, rotation, scale);

        normalizedBindPose = glm::mat4(1.0f);

        float scene_scale = m_Scene->getGlobalSettings()->UnitScaleFactor * 0.01f;
        // Apply translation last
        if (applySceneScale) {
            normalizedBindPose = glm::translate(normalizedBindPose, position * scene_scale);
        } else {
            normalizedBindPose = glm::translate(normalizedBindPose, position);
        }

        normalizedBindPose *= rotation;

        // Apply scaling first
        normalizedBindPose = glm::scale(normalizedBindPose, {1, 1, 1});

        matrix = normalizedBindPose;
    }

    std::shared_ptr<char> FBXImporter::GetTextureDataFromMaterial(const ofbx::Material *material, ofbx::Texture::TextureType textureType, uint32_t &outTextureSize) {
        const ofbx::Texture *texture = material->getTexture(textureType);

        if (texture == nullptr) {
            outTextureSize = 0;
            return nullptr;
        }

        // ofbx::DataView texData = texture->getEmbeddedData();

        auto texFileNameData = texture->getFileName();
        std::string texFileName = DataviewToString(&texFileNameData);

        for (int i = 0, c = m_Scene->getEmbeddedDataCount(); i < c; ++i) {
            const ofbx::DataView embedded_file = m_Scene->getEmbeddedData(i);
            auto embedded_name_data = m_Scene->getEmbeddedFilename(i);
            std::string embedded_name = DataviewToString(&embedded_name_data);

            if (embedded_name == texFileName) {
                if (m_Scene->isEmbeddedBase64(i)) {
                    // V_LOG_ERROR("Pretty plz handle BASE64 embedded files in FBX importer daddy ?");
                    return nullptr;
                } else {
                    uint32_t texDataSize = embedded_file.end - (embedded_file.begin + 4);

                    if (texDataSize == 0) {
                        outTextureSize = 0;
                        return nullptr;
                    }

                    // std::shared_ptr<Mesh>((char *)malloc(texDataSize), std::shared_ptr_MANYMesh::memcpy(textureData.get(), embedded_file.begin + 4, texDataSize);

                    outTextureSize = texDataSize;
                    // return textureData;
                }
            }
        }
    }

    std::string FBXImporter::DataviewToString(ofbx::DataView *view) {
        uint32_t stringSize = view->end - view->begin;

        if (stringSize == 0) {
            return "";
        }

        char *c_str = (char *)malloc(stringSize);
        std::memcpy(c_str, view->begin, stringSize);

        std::string result;
        result.resize(stringSize);

        for (size_t i = 0; i < stringSize; i++) {
            result[i] = c_str[i];
        }
        free(c_str);

        return result;
    }

    void FBXImporter::ComputeTangentsAndBitangents(std::vector<Vertex> &vertexBuffer, const std::vector<uint16_t> &indexBuffer) {
        // Ensure indexBuffer contains valid triangles (must be multiples of 3)
        if (indexBuffer.size() % 3 != 0) {
            throw std::runtime_error("Index buffer size is not a multiple of 3.");
        }

        // Temporary storage for tangents and bitangents
        std::vector<glm::vec3> tangents(vertexBuffer.size(), glm::vec3(0.0f));
        std::vector<glm::vec3> bitangents(vertexBuffer.size(), glm::vec3(0.0f));

        // Iterate over each triangle
        for (size_t i = 0; i < indexBuffer.size(); i += 3) {
            uint16_t i0 = indexBuffer[i];
            uint16_t i1 = indexBuffer[i + 1];
            uint16_t i2 = indexBuffer[i + 2];

            // Retrieve the vertices of the triangle
            Vertex &v0 = vertexBuffer[i0];
            Vertex &v1 = vertexBuffer[i1];
            Vertex &v2 = vertexBuffer[i2];

            // Positions
            glm::vec3 pos0(v0.position.x, v0.position.y, v0.position.z);
            glm::vec3 pos1(v1.position.x, v1.position.y, v1.position.z);
            glm::vec3 pos2(v2.position.x, v2.position.y, v2.position.z);

            // Texture coordinates
            glm::vec2 uv0(v0.texUV.x, v0.texUV.y);
            glm::vec2 uv1(v1.texUV.x, v1.texUV.y);
            glm::vec2 uv2(v2.texUV.x, v2.texUV.y);

            // Calculate the edges of the triangle
            glm::vec3 edge1 = pos1 - pos0;
            glm::vec3 edge2 = pos2 - pos0;

            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            // Calculate the determinant of the UV matrix
            float det = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;
            float f = (det == 0.0f) ? 0.0f : 1.0f / det;

            // Calculate tangent and bitangent
            glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
            glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

            // Accumulate the tangents and bitangents for each vertex
            tangents[i0] += tangent;
            tangents[i1] += tangent;
            tangents[i2] += tangent;

            bitangents[i0] += bitangent;
            bitangents[i1] += bitangent;
            bitangents[i2] += bitangent;
        }

        // Normalize tangents and bitangents and store them in the vertex buffer
        for (size_t i = 0; i < vertexBuffer.size(); ++i) {
            glm::vec3 tangent = glm::normalize(tangents[i]);
            glm::vec3 bitangent = glm::normalize(bitangents[i]);

            vertexBuffer[i].tangent = tangent;
            vertexBuffer[i].bitangent = bitangent;
        }
    }

    glm::mat4 FBXImporter::GetBindPoseMatrix(ImportedBone &bone) {
        const ofbx::Object *node = (const ofbx::Object *)bone.ID;
        const ofbx::Mesh *boneMesh = GetAnyMeshFromBone(node, bone.index);

        if (!boneMesh) {
            return OFBXMatrixToVoidMatrix(node->getGlobalTransform());
        }

        auto *skin = boneMesh->getSkin();
        if (!skin) {
            return OFBXMatrixToVoidMatrix(node->getGlobalTransform());
        }

        for (int i = 0, c = skin->getClusterCount(); i < c; ++i) {
            const ofbx::Cluster *cluster = skin->getCluster(i);
            if (cluster->getLink() == node) {
                return OFBXMatrixToVoidMatrix(cluster->getTransformLinkMatrix());
            }
        }
        return OFBXMatrixToVoidMatrix(node->getGlobalTransform());
    }

} // namespace VaultRenderer
