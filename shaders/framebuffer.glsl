#shader vertex
#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 iTexCoords;

out vec2 texCoords;

void main() {
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
    texCoords = iTexCoords;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screen_texture;
uniform sampler2D bloom_texture;
// uniform sampler2D bloomTexture;
uniform float gamma;
uniform float HDR_Exposure;
uniform bool useBloom;

vec4 bloom() {
    // vec3 color = radialBlur(chromaticAberration(chromaticAmount), vec2(0.5, 0.5), 0.05).rgb;
    vec3 color = texture(screen_texture, texCoords).rgb;
    vec3 bloomColor = texture(bloom_texture, texCoords).rgb;
    return vec4(color + bloomColor, 1.0);
}

void main() {
    vec4 frag = useBloom ? bloom() : texture(screen_texture, texCoords);

    vec3 tone_mapped = vec3(1.0) - exp(-frag.rgb * HDR_Exposure);

    FragColor.rgb = pow(tone_mapped, vec3(1.0 / gamma));
    FragColor.a = 1;
    // FragColor.rgb = texture(bloom_texture, texCoords).rgb;
}