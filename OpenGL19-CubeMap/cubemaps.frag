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
    vec3 R = reflect(I, normalize(Normal));  // ��R�����cube���ĳ����������ĸ����Ƭ�ξͲ�����Ӧ����Ƭ��
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}