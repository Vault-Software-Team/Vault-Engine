#shader vertex
#version 330 core
layout(location = 0) in vec3 vpos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    vec4 pos = projection * view * vec4(vpos, 1.0f);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
    texCoords = vec3(vpos.x, vpos.y, -vpos.z);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;
layout(location = 2) out uint EntityID;

in vec3 texCoords;

uniform samplerCube skybox;

void main() {
    FragColor = texture(skybox, texCoords);
    BloomColor = vec4(0, 0, 0, 0);
    EntityID = uint(0);
}