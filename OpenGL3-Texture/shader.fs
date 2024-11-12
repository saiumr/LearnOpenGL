#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

//uniform sampler2D ourTexture;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float blend;

void main()
{
    //FragColor = vec4(ourColor, 1.0f);
    //FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);

    // default
    //FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.8) * vec4(ourColor, 1.0f);

    // practice1: look another side
    FragColor = mix(texture(texture0, TexCoord), texture(texture1, vec2(1.0 - TexCoord.x, TexCoord.y)), blend) * vec4(ourColor, 1.0f);

}
