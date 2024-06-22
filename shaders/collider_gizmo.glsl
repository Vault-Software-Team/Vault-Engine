#shader vertex
#version 330 core
layout(location = 0) in vec3 vPosition;

uniform mat4 transformModel;
uniform mat4 camera_view;
uniform mat4 camera_projection;

void main() {
    vec3 current_position = vec3(transformModel * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0));
    gl_Position = camera_projection * camera_view * vec4(current_position, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(0, 1, 0, 1);
}