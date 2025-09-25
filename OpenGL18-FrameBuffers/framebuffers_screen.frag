#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform vec2  screenSize;  // ��Ļ�ֱ��ʣ���800x600��
uniform float pixelSize;  // ���ؿ��С����10.0��

void main()
{
    // no post-processing
    /*
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
    */

    // pixelate ���ػ�
    // �������ؿ�����꣨��UV���갴pixelSizeȡ����
    vec2 pixelCoords = floor(TexCoords * screenSize / pixelSize) * pixelSize / screenSize;
    // �����������ĵ���ɫ
    FragColor = texture(screenTexture, pixelCoords);

    // invert color ����
    /*
    FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    */
    
    // grayscale �Ҷ�
    /*
    FragColor = texture(screenTexture, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
    */

    // kernel effect ��Ч��
    // ѡȡ��ͬ�ĺ�ֵ��������ͬЧ����ע�⣬һ�����ܺ����ĺ�ֵ��Ϊ1����Ȼͼ�����ȿ��ܱ仯
    /*
    float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
    );

    // ��
    float kernel_sharp[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    // ģ��
    float kernel_blur[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );

    // ��Ե���
    float kernel_edge[9] = float[](
        1,  1,  1,
        1, -8,  1,
        1,  1,  1  
    );

    // myself
    float kernel_myself[9] = float[](
        1,  1.0/4.0,  1,
        1.0/4.0, -4,  1.0/4,
        1,  1.0/4.0,  1  
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel_myself[i];

    FragColor = vec4(col, 1.0);
    */
} 