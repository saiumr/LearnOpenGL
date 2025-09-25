#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform vec2  screenSize;  // 屏幕分辨率（如800x600）
uniform float pixelSize;  // 像素块大小（如10.0）

void main()
{
    // no post-processing
    /*
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
    */

    // pixelate 像素化
    // 计算像素块的坐标（将UV坐标按pixelSize取整）
    vec2 pixelCoords = floor(TexCoords * screenSize / pixelSize) * pixelSize / screenSize;
    // 采样区块中心的颜色
    FragColor = texture(screenTexture, pixelCoords);

    // invert color 反相
    /*
    FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    */
    
    // grayscale 灰度
    /*
    FragColor = texture(screenTexture, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
    */

    // kernel effect 核效果
    // 选取不同的核值，产生不同效果，注意，一般四周和中心核值和为1，不然图像亮度可能变化
    /*
    float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    // 锐化
    float kernel_sharp[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    // 模糊
    float kernel_blur[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );

    // 边缘检测
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