#shader vertex
#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texUV;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

out vec2 texCoords;

uniform mat4 light_proj;
uniform mat4 transformModel;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main() {
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];
    }
    if(totalPosition.x == 0 && totalPosition.y == 0 && totalPosition.z == 0 && totalPosition.w == 0) {
        totalPosition = vec4(pos, 1);
    }

    gl_Position = light_proj * transformModel * totalPosition;
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
    // if(texture(texture_diffuse.tex, texCoords).r < 0.1) discard;  
}