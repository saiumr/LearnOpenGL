#version 330 core
out vec4 FragColor;

// ����
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D noise;       // �������������ܽ�Ч��
    float shininess;
};

// �����
struct DirectionLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// ���Դ
struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

// �۹� Flashlight
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
uniform float dissolve_threshold;  // �ܽ���ֵ��0.0-1.0�������ܽ�̶�
uniform vec3 edge_color;           // �ܽ��Ե��ɫ
uniform float edge_width;          // �ܽ��Ե��ȣ����Ƹ�����Χ

vec3 CalcDirectionLight(DirectionLight light, vec3 normal, vec3 view_direction);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_direction);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_direction);
vec3 ApplyDissolveEffect(vec3 original_color, vec2 tex_coord);

void main()
{
    // ����
    vec3 surface_normal = normalize(Normal);
    vec3 frag_to_view_direction = normalize(view_pos - FragPos);

    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    // 1.�������
    result += CalcDirectionLight(direction_light, surface_normal, frag_to_view_direction);
    // 2.���Դ
    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        result += CalcPointLight(point_lights[i], surface_normal, FragPos, frag_to_view_direction);
    }
    // 3.�۹�
    result += CalcSpotLight(spotlight, surface_normal, FragPos, frag_to_view_direction);

    // Ӧ���ܽ�Ч��
    result = ApplyDissolveEffect(result, TexCoord);

    FragColor = vec4(result, 1.0f);
}

// �ܽ�Ч��
vec3 ApplyDissolveEffect(vec3 original_color, vec2 tex_coord) {
    // �����������ȡֵ
    float noise_value = texture(material.noise, tex_coord).r;
    
    // ��ȫ�ܽ�Ĳ���ֱ�Ӷ���
    if (noise_value < dissolve_threshold) {
        discard;
    }
    
    // �����ܽ��Ե����Ч��
    // ʹ��smoothstep����ƽ���ı�Ե����
    float edge_factor = smoothstep(
        dissolve_threshold, 
        dissolve_threshold + edge_width, 
        noise_value
    );
    
    // ��Ե��ɫ��ԭʼ��ɫ���
    vec3 edge_highlight = edge_color * (1.0 - edge_factor);
    
    // ��������˱�ԵЧ������ɫ
    return original_color + edge_highlight;
}

vec3 CalcDirectionLight(DirectionLight light, vec3 normal, vec3 view_direction) {
    vec3 reverse_light_direction = normalize(-light.direction);
    // ��������ɫ
    float diff = max(dot(normal, reverse_light_direction), 0.0f);
    // �������ɫ
    vec3 reflect_direction = reflect(-reverse_light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess); 
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_direction) {
    vec3 frag_to_light_direction = normalize(light.position - frag_pos);
    // ��������ɫ
    float diff = max(dot(normal, frag_to_light_direction), 0.0f);
    // �������ɫ
    vec3 reflect_direction = reflect(-frag_to_light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess); 
    // ��ǿ˥��
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

    // ������
    float diff = max(dot(normal, frag_to_light_direction), 0.0f);

    // ���淴��
    vec3 reflect_direction = reflect(-frag_to_light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess);

    // ˥��
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  // Fatt = 1.0 / (Kc + Kd * distance + Kq * distance^2)

    // �۹�ǿ��
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
