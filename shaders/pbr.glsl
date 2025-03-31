#shader vertex
#version 330 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTextureUV;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;

out DATA {
    vec2 texUV;
    vec3 normal;
    vec3 current_position;
    vec4 fragPosLight;
    mat4 cameraCalcs;
    mat4 model;
    vec3 camera_position;
}
data_out;

// out vec3 current_position;
// out vec2 texUV;
// out vec3 normal;
// out vec4 fragPosLight;
// out mat3 TBN;

uniform mat4 transformModel;
uniform mat4 camera_view;
uniform mat4 camera_projection;
uniform mat4 light_proj;
uniform bool mesh_isFlat;
uniform bool playAnimation;
uniform vec2 texUVOffset;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoords;

void main() {
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    if (playAnimation) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            if (boneIds[i] == -1)
                continue;
            if (boneIds[i] >= MAX_BONES) {
                totalPosition = vec4(vPosition, 1.0f);
                break;
            }
            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(vPosition, 1.0f);
            totalPosition += localPosition * weights[i];
            totalNormal += mat3(finalBonesMatrices[boneIds[i]]) * vNormal;
        }
    }

    if ((totalPosition.x == 0 && totalPosition.y == 0 && totalPosition.z == 0 && totalPosition.w == 0) || !playAnimation) {
        totalPosition = vec4(vPosition, 1);
        totalNormal = vNormal;
    }

    data_out.current_position = vec3(transformModel * totalPosition);
    gl_Position = vec4(data_out.current_position, 1.0);
    // Set data_out
    data_out.model = transformModel;
    data_out.cameraCalcs = camera_projection * camera_view;
    data_out.texUV = vTextureUV;
    if (texUVOffset.x > 0) {
        data_out.texUV *= texUVOffset.x;
    }
    if (texUVOffset.y > 0) {
        data_out.texUV *= texUVOffset.y;
    }
    data_out.normal = mat3(transpose(inverse(transformModel))) * totalNormal;
    data_out.normal = normalize(data_out.normal);
    // normal = vNormal;
    // model = transformModel;

    // TBN = mat3(T, B, N);
    // TBN = transpose(TBN);
    data_out.fragPosLight = light_proj * vec4(data_out.current_position, 1);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;
layout(location = 2) out uint EntityID;

const float PI = 3.14159265359;

in vec2 texUV;
in vec3 normal;
// world pos
in vec3 current_position;
in vec4 fragPosLight;
in mat3 TBN;

struct Texture {
    sampler2D tex;
    bool defined;
};

uniform Texture texture_diffuse;
uniform Texture texture_specular;
uniform Texture texture_normal;
uniform Texture texture_height;
uniform Texture texture_roughness;
uniform Texture texture_metallic;
uniform Texture texture_ao;

uniform vec3 camera_position;
uniform sampler2DArray shadowMap;
uniform sampler2D m_shadowMap;
// uniform sampler2D shadowMap;
uniform float ambient_amount;
uniform vec3 ambient_color;
uniform vec4 baseColor;
uniform vec4 emissionColor;
uniform float shadowCubemapFarPlane;
uniform float cameraFarPlane;
uniform uint u_EntityID;
uniform bool u_UseTextureAlpha;

// Shadow Mapping
uniform samplerCube shadowCubemap;
uniform bool shadow_cubemap_mapping;
uniform bool shadow_mapping;
uniform float dirshadow_bias_min;
uniform float dirshadow_bias_max;

// Post Processing Values
struct PostProcessing {
    bool GlobalBloom;
    float BloomThreshold;
    vec3 BloomMultiplier;
    bool PBR_IBL;
    float ShadowStrength;
    vec3 ShadowColor;
};
uniform PostProcessing config_PostProcessing;

// PBR
struct Material {
    float metallic;
    float roughness;
    float ao;
};
uniform Material material;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// Lights
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float shadow_strength;
};

struct DirectionalLight {
    vec3 position;
    vec3 color;
    float intensity;
};

struct SpotLight {
    vec3 position;
    vec3 color;
    vec3 angle;
    float intensity;
};

#define MAX_LIGHTS 100
uniform int point_light_count;
uniform int dir_light_count;
uniform int spot_light_count;
uniform PointLight point_lights[MAX_LIGHTS];
uniform DirectionalLight directional_lights[MAX_LIGHTS];
uniform SpotLight spot_lights[MAX_LIGHTS];
uniform mat4 camera_view;

mat3 _TBN = mat3(vec3(0), vec3(0), vec3(0));

vec3 getNormalFromNormalMap() {
    if (!texture_normal.defined) return normal;
    vec3 tangentNormal = texture(texture_normal.tex, texUV).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(current_position);
    vec3 Q2 = dFdy(current_position);
    vec2 st1 = dFdx(texUV);
    vec2 st2 = dFdy(texUV);

    vec3 N = normalize(normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    _TBN = mat3(T, B, N);

    return normalize(_TBN * tangentNormal);
    // return normalize(TBN * (texture(texture_normal.tex, texUV).xyz * 2.0 - 1.0));
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

const float SHITTY_CONSTANT = 0.00000001;
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), SHITTY_CONSTANT);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

const float MAX_REFLECTION_LOD = 4.0;

layout(std140) uniform LightSpaceMatrices {
    mat4 lightSpaceMatrices[16];
};
uniform float cascadePlaneDistances[16];
uniform int cascadeCount; // number of frusta - 1
uniform vec3 c_ShadowMapLightDir;

float ShadowCalculation(vec3 fragPosWorldSpace, vec3 N, vec3 L, bool cascaded) {
    // select cascade layer
    float shadow = 0.0;
    if (!shadow_mapping) return 0.0;

    vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
    if (lightCoords.z <= 1.0f) {
        lightCoords = (lightCoords + 1.0f) / 2.0f;

        float closestDepth = texture(m_shadowMap, lightCoords.xy).r;
        float currentDepth = lightCoords.z;
        float bias = max(dirshadow_bias_max * (1.0f - dot(N, L)), dirshadow_bias_min);

        int sampleRadius = 2;
        vec2 pixelSize = 1.0 / textureSize(m_shadowMap, 0);
        for (int y = -sampleRadius; y <= sampleRadius; y++) {
            for (int x = -sampleRadius; x <= sampleRadius; x++) {
                float closestDepth = texture(m_shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
                if (currentDepth > closestDepth + bias)
                    shadow += 1.0f;
            }
        }

        shadow /= pow((sampleRadius * 2 + 1), 2);
    }

    return shadow;
}

void main() {
    EntityID = u_EntityID;
    // Load PBR Material Textures
    vec3 albedo = baseColor.rgb;

    if (texture_diffuse.defined) {
        albedo = texture(texture_diffuse.tex, texUV).rgb * baseColor.rgb;
    }

    vec3 normal = getNormalFromNormalMap();
    vec3 WorldPos = current_position;

    // float metallic = texture(texture_metallic, texUV).r;
    // float roughness = texture(texture_roughness, texUV).r;
    // float ao = texture(texture_ao, texUV).r;
    float metallic = material.metallic;
    float roughness = material.roughness;
    float ao = material.ao;

    if (texture_metallic.defined) metallic = texture(texture_metallic.tex, texUV).r;
    if (texture_roughness.defined) roughness = texture(texture_roughness.tex, texUV).r;
    if (texture_ao.defined) ao = texture(texture_ao.tex, texUV).r;

    // do pbr dogshit

    vec3 N = normalize(normal);
    vec3 V = normalize(camera_position - current_position);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, material.metallic);

    vec3 Lo = vec3(0.0);
    // point lights
    float shadow = 0.0f;

    for (int i = 0; i < point_light_count; ++i) {
        vec3 L = normalize(point_lights[i].position - WorldPos);
        vec3 H = normalize(V + L);

        float distance = length(point_lights[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = point_lights[i].color * attenuation * point_lights[i].intensity;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), SHITTY_CONSTANT) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;

        if (shadow_cubemap_mapping) {
            vec3 fragToLight = current_position - (point_lights[i].position);
            float current_depth = length(fragToLight);
            float bias = max(0.025 * (1.0 - dot(normal, L)), 0.0005);

            int sampleRadius = 2;
            float pixelSize = 1.0 / 1024;

            for (int z = -sampleRadius; z <= sampleRadius; z++) {
                for (int y = -sampleRadius; y <= sampleRadius; y++) {
                    for (int x = -sampleRadius; x <= sampleRadius; x++) {
                        float closestDepth = texture(shadowCubemap, fragToLight + vec3(x, y, z) * pixelSize).r;
                        closestDepth *= shadowCubemapFarPlane;
                        if (current_depth > closestDepth + bias) shadow += 1.0;
                    }
                }
            }
            shadow /= pow((sampleRadius * 2 + 1), 3);
        }
    }

    // dir lights
    for (int i = 0; i < dir_light_count; ++i) {
        vec3 L = normalize(directional_lights[i].position);
        vec3 H = normalize(V + L);

        float distance = length(directional_lights[i].position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = directional_lights[i].color * directional_lights[i].intensity;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), SHITTY_CONSTANT) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;

        if (shadow_mapping) {
            shadow = ShadowCalculation(current_position, N, L, false);
        }
    }

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 color = vec3(0);
    if (config_PostProcessing.PBR_IBL) {
        vec3 ambient = (kD * diffuse + specular) * ao;
        ambient *= ambient_amount;
        color = (ambient + Lo) * (1.2 - (shadow * config_PostProcessing.ShadowStrength));
    } else {
        vec3 ambient = vec3(ambient_amount) * albedo * ao;
        color = (ambient + Lo) * (1.2 - (shadow * config_PostProcessing.ShadowStrength));
    }
    // color = color / (color + vec3(1.0));
    // color = pow(color, vec3(1.0 / 2.2));

    // end pbr dogshit (finally)

    FragColor.rgb = color;
    if (texture_diffuse.defined) {
        FragColor.a = texture(texture_diffuse.tex, texUV).a * baseColor.a;
    } else {
        FragColor.a = baseColor.a;
    }

    if (config_PostProcessing.GlobalBloom) {
        float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
        if (emissionColor.rgb == vec3(0, 0, 0)) {
            if (brightness > config_PostProcessing.BloomThreshold) {
                BloomColor = FragColor * vec4(config_PostProcessing.BloomMultiplier, 1);
            } else {
                BloomColor = vec4(0, 0, 0, FragColor.a);
            }
        } else {
            BloomColor = vec4(emissionColor.rgb, FragColor.a);
        }
    } else {
        BloomColor = vec4(emissionColor.rgb, FragColor.a);
    }

    // BloomColor = vec4(5, 5, 5, 1);
}

#shader geometry
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec2 texUV;
out vec3 normal;
out vec3 current_position;
out vec4 fragPosLight;
out mat3 TBN;
out vec3 camera_position;

in DATA {
    vec2 texUV;
    vec3 normal;
    vec3 current_position;
    vec4 fragPosLight;
    mat4 cameraCalcs;
    mat4 model;
    vec3 camera_position;
}
data_in[];

void main() {
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec2 deltaUV0 = data_in[1].texUV - data_in[0].texUV;
    vec2 deltaUV1 = data_in[2].texUV - data_in[0].texUV;

    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(data_in[0].model * vec4(data_in[1].normal, 0.0)));

    // T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    // vec3 B = cross(N, T);

    mat3 mTBN = mat3(T, B, N);
    // mTBN = transpose(mTBN);

    gl_Position = data_in[0].cameraCalcs * gl_in[0].gl_Position;
    texUV = data_in[0].texUV;
    normal = data_in[0].normal;
    current_position = data_in[0].current_position;
    fragPosLight = data_in[0].fragPosLight;
    camera_position = data_in[0].camera_position;
    TBN = mTBN;
    EmitVertex();

    gl_Position = data_in[1].cameraCalcs * gl_in[1].gl_Position;
    texUV = data_in[1].texUV;
    normal = data_in[1].normal;
    current_position = data_in[1].current_position;
    fragPosLight = data_in[1].fragPosLight;
    camera_position = data_in[1].camera_position;
    TBN = mTBN;
    EmitVertex();

    gl_Position = data_in[2].cameraCalcs * gl_in[2].gl_Position;
    texUV = data_in[2].texUV;
    normal = data_in[2].normal;
    current_position = data_in[2].current_position;
    fragPosLight = data_in[2].fragPosLight;
    camera_position = data_in[2].camera_position;
    TBN = mTBN;
    EmitVertex();

    EndPrimitive();
}