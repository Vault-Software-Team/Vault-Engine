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

out vec2 TexCoords;

void main() {
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);

    totalPosition = vec4(vPosition, 1);
    totalNormal = vNormal;

    current_position = vec3(transformModel * totalPosition);
    gl_Position = (camera_projection * camera_view) * vec4(current_position, 1.0);
    // Set data_out
    // cameraCalcs = camera_projection * camera_view;
    texUV = vTextureUV;
    normal = normalize(normal);
    // normal = vNormal;
    // model = transformModel;

    // TBN = mat3(T, B, N);
    // TBN = transpose(TBN);
    fragPosLight = light_proj * vec4(current_position, 1);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;
layout(location = 2) out uint EntityID;

in vec2 texUV;
in vec3 normal;
in vec3 current_position;
in vec4 fragPosLight;

struct Texture {
    sampler2D tex;
    bool defined;
};

uniform Texture texture_diffuse;

uniform vec4 baseColor;
uniform uint u_EntityID;

void main() {
    FragColor = texture(texture_diffuse.tex, texUV);
    BloomColor = vec4(1, 1, 1, 1);
    EntityID = u_EntityID;
}