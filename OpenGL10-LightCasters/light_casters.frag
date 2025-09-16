#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 view_pos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    float cut_off;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;

void main()
{
    vec3 light_direction = normalize(light.position - FragPos);  // light_direction is diretion of Frag -> light cube
    
    // 聚光计算（摄像机为射点，但是光强还要用cube提供）
    // 主要是模拟手电筒视角
    float theta = dot(light_direction, normalize(-light.direction));
    if (theta > light.cut_off) {  // 范围之内正常显示
        // 环境光
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

        // 漫反射
        vec3 light_surface_normal = normalize(Normal);
        float diff = max(dot(light_surface_normal, light_direction), 0.0f);  // dot(normal1, normal2) = cos(degree)
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

        // 镜面光
        vec3  view_direction = normalize(view_pos - FragPos);
        vec3  light_reflect_direction = reflect(-light_direction, light_surface_normal);
        float spec = pow(max(dot(view_direction, light_reflect_direction), 0.0f), material.shininess);
        vec3  specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

        // 计算光强衰减
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + 
                    light.quadratic * (distance * distance));  // Fatt = 1.0 / (Kc + Kd * distance + Kq * distance^2)

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0f);
    } else {
        // 范围之外只有环境光
        FragColor = vec4(light.ambient * vec3(texture(material.diffuse, TexCoord)), 1.0f);
    }


}
