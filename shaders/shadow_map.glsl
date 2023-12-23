#shader vertex
#version 330 core
layout(location = 0) in vec3 pos;

uniform mat4 light_proj;
uniform mat4 transformModel;

void main() {
    gl_Position = light_proj * transformModel * vec4(pos, 1.0);
}
#shader fragment
#version 330 core

void main() {
    
}