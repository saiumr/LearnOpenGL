#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float blend;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 light_pos;
uniform vec3 view_pos;

void main()
{
    // practice1: look another side
    // FragColor = mix(texture(texture0, TexCoord), texture(texture1, vec2(1.0 - TexCoord.x, TexCoord.y)), blend);

    float ambient_strength = 0.2;
    vec3 ambient = ambient_strength * lightColor;

    vec3 light_surface_normal = normalize(Normal);
    vec3 light_direction = normalize(light_pos - FragPos);
    float diff = max(dot(light_surface_normal, light_direction), 0.0f);  // dot(normal1, normal2) = cos(degree)
    vec3 diffuse = diff * lightColor;

    float specular_strength = 0.5;
    vec3  view_direction = normalize(view_pos - FragPos);
    vec3  light_reflect_direction = reflect(-light_direction, light_surface_normal);
    float spec = pow(max(dot(view_direction, light_reflect_direction), 0.0f), 32);  // 32 is object's shininess'
    vec3  specular = specular_strength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0f);


}
