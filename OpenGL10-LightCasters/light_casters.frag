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

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main()
{
    // 环境光
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // 漫反射
    vec3 light_surface_normal = normalize(Normal);
    vec3 light_direction = normalize(light.position - FragPos);
    float diff = max(dot(light_surface_normal, light_direction), 0.0f);  // dot(normal1, normal2) = cos(degree)
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    // 镜面光
    vec3  view_direction = normalize(view_pos - FragPos);
    vec3  light_reflect_direction = reflect(-light_direction, light_surface_normal);
    float spec = pow(max(dot(view_direction, light_reflect_direction), 0.0f), material.shininess);
    vec3  specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}
