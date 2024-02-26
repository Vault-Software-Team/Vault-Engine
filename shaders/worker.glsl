#shader vertex
#version 330 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTextureUV;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;

out vec2 texUV;
out vec3 normal;
out vec3 current_position;
out vec4 fragPosLight;
out vec3 camera_position;

// out vec3 current_position;
// out vec2 texUV;
// out vec3 normal;
// out vec4 fragPosLight;
// out mat3 TBN;

uniform mat4 transformModel;
uniform mat4 camera_view;
uniform mat4 camera_projection;
uniform mat4 light_proj;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoords;

void main() {
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);

    totalPosition = vec4(vPosition, 1);
    totalNormal = vNormal;

    current_position = vec3(transformModel * totalPosition);
    gl_Position = vec4(current_position, 1.0);
    // Set data_out
    texUV = vTextureUV;
    normal = mat3(transpose(inverse(transformModel))) * totalNormal;
    normal = normalize(normal);
    // normal = vNormal;
    // model = transformModel;

    // TBN = mat3(T, B, N);
    // TBN = transpose(TBN);
    fragPosLight = light_proj * vec4(current_position, 1);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 texUV;
in vec3 normal;
in vec3 current_position;
in vec4 fragPosLight;

uniform vec4 baseColor;

void main() {
    FragColor = vec4(1, 0, 0, 1);
    FragColor.a = 1;
}