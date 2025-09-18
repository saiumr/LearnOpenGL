#version 330 core
out vec4 FragColor;

// 材质
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

// 定向光
struct DirectionLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// 点光源
struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

// 聚光 Flashlight
struct SpotLight {
    vec3 position;
    vec3 direction;
    float inner_cut_off;
    float outer_cut_off;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 view_pos;
uniform DirectionLight direction_light;
uniform Material material;
uniform PointLight point_lights[NR_POINT_LIGHTS];
uniform SpotLight spotlight;

vec3 CalcDirectionLight(DirectionLight light, vec3 normal, vec3 view_direction);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_direction);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_direction);

void main()
{
    // 属性
    vec3 surface_normal = normalize(Normal);
    vec3 frag_to_view_direction = normalize(view_pos - FragPos);

    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    // 1.定向光照
    result += CalcDirectionLight(direction_light, surface_normal, frag_to_view_direction);
    // 2.点光源
    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        result += CalcPointLight(point_lights[i], surface_normal, FragPos, frag_to_view_direction);
    }
    // 3.聚光
    result += CalcSpotLight(spotlight, surface_normal, FragPos, frag_to_view_direction);

    FragColor = vec4(result, 1.0f);
}

vec3 CalcDirectionLight(DirectionLight light, vec3 normal, vec3 view_direction) {
    vec3 reverse_light_direction = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, reverse_light_direction), 0.0f);
    // 镜面光着色
    vec3 reflect_direction = reflect(-reverse_light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess); 
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_direction) {
    vec3 frag_to_light_direction = normalize(light.position - frag_pos);
    // 漫反射着色
    float diff = max(dot(normal, frag_to_light_direction), 0.0f);
    // 镜面光着色
    vec3 reflect_direction = reflect(-frag_to_light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess); 
    // 光强衰减
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  // Fatt = 1.0 / (Kc + Kd * distance + Kq * distance^2)

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_direction) {
    vec3 frag_to_light_direction = normalize(light.position - frag_pos);

    // 漫反射
    float diff = max(dot(normal, frag_to_light_direction), 0.0f);

    // 镜面反射
    vec3 reflect_direction = reflect(-frag_to_light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess);

    // 衰减
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  // Fatt = 1.0 / (Kc + Kd * distance + Kq * distance^2)

    // 聚光强度
    float theta = dot(frag_to_light_direction, normalize(-light.direction));
    float epsilon = light.inner_cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0f, 1.0f);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    // ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}
