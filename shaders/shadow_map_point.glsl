#shader vertex
#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 txuv;

out vec2 texCoords;

uniform mat4 transformModel;

void main() {
    gl_Position = transformModel * vec4(pos, 1.0);
    texCoords = txuv;
}
#shader fragment
#version 330 core

in vec4 FragPos;
uniform vec3 light_position;
uniform float far;

in vec2 texCoords;

struct Texture {
    sampler2D tex;
    bool defined;
};
uniform Texture texture_diffuse;

void main() {
    if(texture(texture_diffuse.tex, texCoords).r < 0.1) discard;  
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