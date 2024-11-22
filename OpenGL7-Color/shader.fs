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

void main()
{
    // practice1: look another side
    // FragColor = mix(texture(texture0, TexCoord), texture(texture1, vec2(1.0 - TexCoord.x, TexCoord.y)), blend);

    float ambient_strength = 0.2;
    vec3 ambient = ambient_strength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_pos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);  // dot(normal1, normal2) = cos(degree)
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0f);


}
