#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D texture0;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{             
    // FragColor = texture(texture0, TexCoords);
    vec3 I = normalize(Position - cameraPos);
    
    // reflection
    // vec3 R = reflect(I, normalize(Normal));  // 把R想象从cube中心出发，击中哪个面的片段就采样对应纹理片段
    
    // refraction
    float ratio = 1.00 / 1.52;
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}