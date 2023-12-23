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

struct Texture {
    sampler2D tex;
    bool defined;
};

uniform Texture texture_diffuse;
uniform Texture texture_specular;
uniform float ambient_amount;
uniform vec3 ambient_color;
uniform vec3 camera_position;
uniform vec4 baseColor;
vec3 m_normal = normalize(normal);

// Lights
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    vec3 position;
    vec3 color;
    float intensity;
};

struct SpotLight {
    vec3 position;
    vec3 color;
    vec3 angle;
    float intensity;
};

#define MAX_LIGHTS 100
uniform PointLight point_lights[MAX_LIGHTS];
uniform DirectionalLight directional_lights[MAX_LIGHTS];
uniform SpotLight spot_lights[MAX_LIGHTS];

vec4 ambient() {
    return vec4(ambient_color * ambient_amount, 1.0f);
}

vec3 point_light(PointLight light) {
    vec3 light_vec = light.position - current_position;
    float dist = length(light_vec);
    float a = 1.0;
    float b = 0.04;
    float inten = light.intensity / (a * dist * dist + b * dist + 1.0f);

    vec3 light_dir = normalize(light_vec);

    float diffuse = max(dot(m_normal, light_dir), 0.0f);

    float specular_light = 0.5;

    vec3 view_dir = normalize(camera_position - current_position);
    vec3 reflection_dir = reflect(-light_dir, m_normal);
    vec3 halfway_vec = normalize(view_dir + light_dir);
    float specular_amount = pow(max(dot(m_normal, halfway_vec), 0.0), 16);

    float specular = specular_amount * specular_light;

    float spec = 0;
    if(texture_specular.defined) {
        spec = texture(texture_specular.tex, texUV).r * specular;
    } else {
        spec = specular;
    }
    spec *= inten;

    if(texture_diffuse.defined) {

        return (texture(texture_diffuse.tex, texUV).rgb * baseColor.rgb) * light.color * (diffuse * inten + ambient_amount) + (spec);
    } else {
        return (baseColor.rgb) * light.color * (diffuse * inten + ambient_amount) + (spec);
    }
}

vec3 directional_light(DirectionalLight light) {
    float inten = light.intensity;
    vec3 light_dir = normalize(light.position);

    float diffuse = max(dot(m_normal, light_dir), 0.0f);

    float specular_light = 0.5;

    vec3 view_dir = normalize(camera_position - current_position);
    vec3 reflection_dir = reflect(-light_dir, m_normal);

    vec3 halfway_vec = normalize(view_dir + light_dir);

    float specular_amount = pow(max(dot(m_normal, halfway_vec), 0.0), 16);
    float specular = specular_amount * specular_light;

    float spec = 0;
    if(texture_specular.defined) {
        spec = texture(texture_specular.tex, texUV).r * specular;
    } else {
        spec = specular;
    }
    spec *= inten;

    if(texture_diffuse.defined) {

        return (texture(texture_diffuse.tex, texUV).rgb * baseColor.rgb) * light.color * (diffuse * inten + ambient_amount) + (spec);
    } else {
        return (baseColor.rgb) * light.color * (diffuse * inten + ambient_amount) + (spec);
    }

}

vec3 spot_light(SpotLight light) {
    float outer_cone = 0.90;
    float inner_cone = 0.95;

    vec3 light_vec = light.position - current_position;
    vec3 light_dir = normalize(light_vec);
    float diffuse = max(dot(m_normal, light_dir), 0.0);

    float specular_light = 0.5;

    vec3 view_dir = normalize(camera_position - current_position);
    vec3 reflection_dir = reflect(-light_dir, m_normal);

    vec3 halfway_vec = normalize(view_dir + light_dir);
    float specular_amount = pow(max(dot(m_normal, halfway_vec), 0.0), 16);

    float specular = specular_amount * specular_light;

    float spec = 0;
    if(texture_specular.defined) {
        spec = texture(texture_specular.tex, texUV).r * specular;
    } else {
        spec = specular;
    }
   
    float angle = dot(light.angle, -light_dir);
    float inten = clamp((angle - outer_cone) / (inner_cone - outer_cone), 0.0, 1.0);
    spec *= inten;

    if(texture_diffuse.defined) {

        return (texture(texture_diffuse.tex, texUV).rgb * baseColor.rgb) * light.color * (diffuse * inten + ambient_amount) + (spec);
    } else {
        return (baseColor.rgb) * light.color * (diffuse * inten + ambient_amount) + (spec);
    }
}

void main()
{
    vec4 total_color = vec4(0);
    for(int i = 0; i < MAX_LIGHTS; i++) {
        if(point_lights[i].intensity > 0)
            total_color += vec4(point_light(point_lights[i]), 1.0f);

        if(directional_lights[i].intensity > 0)
            total_color += vec4(directional_light(directional_lights[i]), 1.0f);

        if(spot_lights[i].intensity > 0)
            total_color += vec4(spot_light(spot_lights[i]), 1.0f);
    }

    if(total_color.r == 0 && total_color.g == 0 && total_color.b == 0) {
        total_color = vec4(texture(texture_diffuse.tex, texUV).rgb * baseColor.rgb * ambient_amount, 1);
    }
    
    if(texture_diffuse.defined) {
        total_color.a = texture(texture_diffuse.tex, texUV).a * baseColor.a;
    } else {
        total_color.a = baseColor.a;
    }
    // PointLight light;
    // light.position = vec3(1.0, 0.3, 1);
    // light.color = vec3(1,1,1);
    // light.intensity = 1.0;
    // total_color += vec4(point_light(light), 1.0f);

    FragColor = total_color;
} 