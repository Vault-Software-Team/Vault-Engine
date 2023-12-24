#shader vertex
#version 330 core
layout(location = 0) in vec3 pos;

uniform mat4 transformModel;

void main() {
    gl_Position = transformModel * vec4(pos, 1.0);
}
#shader fragment
#version 330 core

in vec4 FragPos;
uniform vec3 light_position;
uniform float far;

void main() {
    gl_FragDepth = length(FragPos.xyz - light_position) / far;
}

#shader geometry
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

uniform mat4 shadow_matrices[6];
out vec4 FragPos;

void main() {
    for(int f = 0; f < 6; ++f) {
        gl_Layer = f;
        for(int i = 0; i < 3; i++) {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadow_matrices[f] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}