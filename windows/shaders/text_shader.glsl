#shader vertex
#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texUV;
out vec2 TexCoords;

uniform mat4 camera_view;
uniform mat4 camera_projection;
uniform mat4 transformModel;

void main()
{
    gl_Position = camera_projection * camera_view * transformModel * vec4(pos.xy, 0.0, 1.0);
    TexCoords = texUV;
}  

#shader fragment
#version 330 core
out vec4 color;
out vec4 BloomColor;
out uint EntityID;

in vec2 TexCoords;

struct Texture {
    sampler2D tex;
    bool defined;
};

uniform Texture texture_diffuse;
uniform vec3 textColor;
uniform vec3 bloomColor;
uniform uint u_EntityID;

void main()
{    
    if(texture(texture_diffuse.tex, TexCoords).r < 0.1) discard;
    vec4 result = vec4(textColor, texture(texture_diffuse.tex, TexCoords).r);
    color = result;
    EntityID = u_EntityID;

    if((bloomColor.r > 0.5 || bloomColor.g > 0.5 || bloomColor.b > 0.5)
    && (bloomColor.r < 0.7 || bloomColor.g < 0.7 || bloomColor.b < 0.7)) {
        color = result * (bloomColor.r * 20);
        BloomColor = vec4(bloomColor * 2, 1);
    } else if(bloomColor.r > 0.7 || bloomColor.g > 0.7 || bloomColor.b > 0.7) {
        color = result * (bloomColor.r * 30);
        BloomColor = vec4(bloomColor * 3, 1);
    } else {
        BloomColor = vec4(bloomColor, 1);
    }
    
    color.a = texture(texture_diffuse.tex, TexCoords).r;
}