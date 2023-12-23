#shader vertex
#version 330 core
layout (location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTextureUV;
layout(location = 2) in vec3 vNormal;

out vec2 texUV;
out vec3 normal;
out vec3 current_position;
uniform mat4 transformModel;
uniform mat4 camera_view;
uniform mat4 camera_projection;

void main()
{
    current_position = vec3(transformModel * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0));
    gl_Position = camera_projection * camera_view * vec4(current_position, 1.0);
    texUV = vTextureUV;
    normal = vNormal;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 texUV;
in vec3 normal;
in vec3 current_position;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform float ambient_amount;
uniform vec3 ambient_color;
uniform vec3 camera_position;
vec3 m_normal = normalize(normal);

// Lights
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

#define MAX_LIGHTS 100
uniform PointLight point_lights[MAX_LIGHTS];

vec4 ambient() {
    return vec4(ambient_color * ambient_amount, 1.0f);
}

vec3 point_light(PointLight light) {
    vec3 light_dir = normalize(light.position - current_position);
    float diffuse = max(dot(m_normal, light_dir), 0.0f);

    float specular_light = 0.5;

    vec3 view_dir = normalize(camera_position - current_position);
    vec3 reflection_dir = reflect(-light_dir, m_normal);
    float specular_amount = pow(max(dot(view_dir, reflection_dir), 0.0), 16);

    float specular = specular_amount * specular_light;

    return texture(texture_diffuse, texUV).rgb * light.color * (diffuse + ambient_amount) + (texture(texture_specular, texUV).r * specular);
}

void main()
{
    vec4 total_color = vec4(0);
    total_color.a = 1;
    for(int i = 0; i < MAX_LIGHTS; i++) {
        if(point_lights[i].intensity > 0)
            total_color += vec4(point_light(point_lights[i]), 1.0f);
    }

    if(total_color.r == 0 && total_color.g == 0 && total_color.b == 0) {
        total_color = vec4(texture(texture_diffuse, texUV).rgb * ambient_amount, 1);
    }
    // PointLight light;
    // light.position = vec3(1.0, 0.3, 1);
    // light.color = vec3(1,1,1);
    // light.intensity = 1.0;
    // total_color += vec4(point_light(light), 1.0f);

    FragColor = total_color;
} 