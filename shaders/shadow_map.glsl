#shader vertex
#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texUV;

out vec2 texCoords;

uniform mat4 light_proj;
uniform mat4 transformModel;

void main() {
    gl_Position = light_proj * transformModel * vec4(pos, 1.0);
    texCoords = texUV;
}

#shader fragment
#version 330 core

in vec2 texCoords;

struct Texture {
    sampler2D tex;
    bool defined;
};
uniform Texture texture_diffuse;

void main() {
    if(texture(texture_diffuse.tex, texCoords).r < 0.1) discard;  
}