#shader vertex
#version 330 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTex;

uniform mat4 transformModel;
uniform mat4 camera_view;
uniform mat4 camera_projection;

out vec2 texUV;

void main() {
    vec3 current_position = vec3(transformModel * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0));
    gl_Position = camera_projection * camera_view * vec4(current_position, 1.0);

    texUV = vTex;
}

#shader fragment
#version 330 core
out vec4 FragColor;
out vec4 BloomColor;
out uint EntityID;

in vec2 texUV;
uniform uint u_EntityID;

struct Texture {
    sampler2D tex;
    bool defined;
};

uniform Texture texture_diffuse;

void main() {
    EntityID = u_EntityID;
    BloomColor = vec4(0, 0, 0, 1);
    FragColor = texture_diffuse.defined ? texture(texture_diffuse.tex, texUV) : vec4(0, 1, 0, 1);
}