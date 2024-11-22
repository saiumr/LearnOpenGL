#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float blend;

vec3 LightingColor = vec3(1.0f, 1.0f, 1.0f);
vec3 StuffColor = vec3(1.0f, 0.5f, 0.31f);

void main()
{
    // practice1: look another side
    // FragColor = mix(texture(texture0, TexCoord), texture(texture1, vec2(1.0 - TexCoord.x, TexCoord.y)), blend);
    FragColor = vec4(LightingColor * StuffColor, 1.0f);
}
