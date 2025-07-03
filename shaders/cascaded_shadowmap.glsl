#shader vertex
#version 400 core
layout(location = 0) in vec3 aPos;

uniform mat4 transformModel;
void main() {
    gl_Position = transformModel * vec4(aPos, 1.0);
}

#shader fragment
#version 400 core

void main() {
}

#shader geometry
#version 400 core

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

layout(std140) uniform LightSpaceMatrices {
    mat4 lightSpaceMatrices[16];
};

void main() {
    for (int i = 0; i < 3; ++i) {
        gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}