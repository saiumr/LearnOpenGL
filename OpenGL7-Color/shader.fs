#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float blend;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    // practice1: look another side
    // FragColor = mix(texture(texture0, TexCoord), texture(texture1, vec2(1.0 - TexCoord.x, TexCoord.y)), blend);

    float ambient_strength = 0.2;
    vec3 ambient = ambient_strength * lightColor;

    vec3 result = ambient * objectColor;
    FragColor = vec4(result, 1.0f);
}
