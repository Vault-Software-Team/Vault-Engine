#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 WorldPos;

void main() {
    WorldPos = aPos;

    mat4 rotView = mat4(mat3(view));
    vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);

    gl_Position = clipPos.xyww;
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main() {
    vec3 envColor = textureLod(environmentMap, WorldPos, 1.2).rgb;

    FragColor = vec4(envColor, 1.0);
}