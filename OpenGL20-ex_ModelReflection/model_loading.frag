#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = texture(texture_specular1, TexCoords);
    //FragColor = texture(texture_normal1, TexCoords);
    //FragColor = texture(texture_height1, TexCoords);

    vec3 model_color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vec3 reflect_color = texture(skybox, R).rgb;

    float reflect_strength = 0.5;
    vec3  result = mix(model_color, reflect_color, reflect_strength);

    FragColor = vec4(result, 1.0);
}