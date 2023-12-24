#shader vertex
#version 330 core
layout (location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTextureUV;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out DATA
{
    vec2 texUV;
    vec3 normal;
    vec3 current_position;
    vec4 fragPosLight;
    mat4 cameraCalcs;
    mat4 model;
} data_out;

uniform mat4 transformModel;
uniform mat4 camera_view;
uniform mat4 camera_projection;
uniform mat4 light_proj;

void main()
{
    data_out.current_position = vec3(transformModel * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0));
    gl_Position = vec4(data_out.current_position, 1.0);

    // Set data_out
    data_out.cameraCalcs = camera_projection * camera_view;
    data_out.texUV = vTextureUV;
    data_out.normal = normalize(vNormal);
    data_out.model = transformModel;

    // data_out.TBN = mat3(T, B, N);
    // TBN = transpose(TBN);
    data_out.fragPosLight = light_proj * vec4(data_out.current_position, 1);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 texUV;
in vec3 normal;
in vec3 current_position;
in vec4 fragPosLight;
in mat3 TBN;

struct Texture {
    sampler2D tex;
    bool defined;
};

uniform Texture texture_diffuse;
uniform Texture texture_specular;
uniform Texture texture_normal;
uniform sampler2D shadowMap;
uniform float ambient_amount;
uniform vec3 ambient_color;
uniform vec3 camera_position;
uniform vec4 baseColor;
uniform samplerCube shadowCubemap;
uniform bool shadow_cubemap_mapping;
uniform bool shadow_mapping;
uniform float shadowCubemapFarPlane;
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
uniform int point_light_count;
uniform int dir_light_count;
uniform int spot_light_count;
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

    float shadow = 0.0;
    // if(false) {
    //     vec3 fragToLight = current_position - light.position;
    //     float current_depth = length(fragToLight);
    //     float bias = max(0.5 * (1.0 - dot(m_normal, light_dir)), 0.0005);

    //     int sampleRadius = 2;
    //     float pixelSize = 1.0 / 1024;

    //     for(int z = -sampleRadius; z <= sampleRadius; z++) {
    //         for (int y = -sampleRadius; y <= sampleRadius; y++) {
    //             for (int x = -sampleRadius; x <= sampleRadius; x++) {
    //                 float closestDepth = texture(shadowCubemap, fragToLight + vec3(x,y,z) * pixelSize).r;
    //                 closestDepth *= shadowCubemapFarPlane;
    //                 if(current_depth > closestDepth + bias) shadow += 1.0;
    //             }
    //         }
    //     }
    //     shadow /= pow((sampleRadius * 2 + 1), 3);
    // }

    if(texture_diffuse.defined) {
        return (texture(texture_diffuse.tex, texUV).rgb * baseColor.rgb) * light.color * (diffuse * (1.0 - shadow) * inten + ambient_amount) + (spec * (1.0 - shadow));
    } else {
        return (baseColor.rgb) * light.color * (diffuse * (1.0 - shadow) * inten + ambient_amount) + (spec * (1.0 - shadow));
    }
}

vec3 directional_light(DirectionalLight light) {
    float inten = light.intensity;
    vec3 light_dir = normalize(light.position);

    vec3 t_normal = vec3(0);
    if(false) {
        t_normal = texture(texture_normal.tex, texUV).rgb;
        t_normal = t_normal * 2.0 - 1.0;
        t_normal = normalize(t_normal);
        
    } else {
        t_normal = m_normal;
    }
    float diffuse = max(dot(t_normal, light_dir), 0.0f);

    float specular_light = 0.5;


    vec3 view_dir = normalize(camera_position - current_position);
    vec3 reflection_dir = reflect(-light_dir, t_normal);

    vec3 halfway_vec = normalize(view_dir + light_dir);

    float specular_amount = pow(max(dot(t_normal, halfway_vec), 0.0), 16);
    float specular = specular_amount * specular_light;

    float spec = 0;
    if(texture_specular.defined) {
        spec = texture(texture_specular.tex, texUV).r * specular;
    } else {
        spec = specular;
    }
    spec *= inten;

    // float shadow = 0.0;
    // vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
    // if(lightCoords.z <= 1.0) {
    //     lightCoords = (lightCoords + 1.0) / 2.0;

    //     float closestDepth = texture(shadowMap, lightCoords.xy).r;
    //     float currentDepth = lightCoords.z;

    //     float bias = 0.005;
    //     if(currentDepth > closestDepth + bias) {
    //         shadow = 1;
    //     }
    // } 

    float shadow = 0.0f;
    if(shadow_mapping) {
        vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
        if(lightCoords.z <= 1.0f) {
            lightCoords = (lightCoords + 1.0f) / 2.0f;

            float closestDepth = texture(shadowMap, lightCoords.xy).r;
            float currentDepth = lightCoords.z;
            float bias = max(0.025f * (1.0f - dot(t_normal, light_dir)), 0.0005f);

            int sampleRadius = 2;
            vec2 pixelSize  = 1.0 / textureSize(shadowMap, 0);
            for(int y = -sampleRadius; y <= sampleRadius; y++) {
                for(int x = -sampleRadius; x <= sampleRadius; x++) {
                    float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x,y) * pixelSize).r;
                    if(currentDepth > closestDepth + bias)
                        shadow += 1.0f;
                }
            }

            shadow /= pow((sampleRadius * 2 + 1), 2);
        }
    }

    if(texture_diffuse.defined) {
        return (texture(texture_diffuse.tex, texUV).rgb * baseColor.rgb) * light.color * (diffuse * (1.0 - shadow) * inten + ambient_amount) + (spec * (1.0 - shadow));
    } else {
        return (baseColor.rgb) * light.color * (diffuse * (1.0 - shadow) * inten + ambient_amount) + (spec * (1.0 - shadow));
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
    for(int i = 0; i < point_light_count; i++) {
        if(point_lights[i].intensity > 0)
            total_color += vec4(point_light(point_lights[i]), 1.0f);

        if(directional_lights[i].intensity > 0)
            total_color += vec4(directional_light(directional_lights[i]), 1.0f);

        if(spot_lights[i].intensity > 0)
            total_color += vec4(spot_light(spot_lights[i]), 1.0f);
    }

    for(int i = 0; i < dir_light_count; i++) {
        if(directional_lights[i].intensity > 0)
            total_color += vec4(directional_light(directional_lights[i]), 1.0f);
    }

    for(int i = 0; i < spot_light_count; i++) {
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

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    
} 
#shader geometry
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec2 texUV;
out vec3 normal;
out vec3 current_position;
out vec4 fragPosLight;
out mat3 TBN;

in DATA
{
    vec2 texUV;
    vec3 normal;
    vec3 current_position;
    vec4 fragPosLight;
    mat4 cameraCalcs;
    mat4 model;
} data_in[];

void main() {
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec2 deltaUV0 = data_in[1].texUV - data_in[0].texUV;
    vec2 deltaUV1 = data_in[2].texUV - data_in[0].texUV;

    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(data_in[0].model * vec4(cross(edge1, edge0), 0.0)));

    mat3 mTBN = mat3(T, B, N);
    mTBN = transpose(mTBN);

    gl_Position = data_in[0].cameraCalcs * gl_in[0].gl_Position;
    texUV = data_in[0].texUV;
    normal = data_in[0].normal;
    current_position = data_in[0].current_position;
    fragPosLight = data_in[0].fragPosLight;
    TBN = mTBN;
    EmitVertex();

    gl_Position = data_in[1].cameraCalcs * gl_in[1].gl_Position;
    texUV = data_in[1].texUV;
    normal = data_in[1].normal;
    current_position = data_in[1].current_position;
    fragPosLight = data_in[1].fragPosLight;
    TBN = mTBN;
    EmitVertex();

    gl_Position = data_in[2].cameraCalcs * gl_in[2].gl_Position;
    texUV = data_in[2].texUV;
    normal = data_in[2].normal;
    current_position = data_in[2].current_position;
    fragPosLight = data_in[2].fragPosLight;
    TBN = mTBN;
    EmitVertex();

    EndPrimitive();
}