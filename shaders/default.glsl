#shader vertex
#version 330 core
layout (location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTextureUV;

out vec2 texUV;
uniform mat4 transformModel;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * transformModel * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
    texUV = vTextureUV;
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 texUV;

uniform sampler2D texture_diffuse;

void main()
{
    FragColor = texture(texture_diffuse, texUV);
} 