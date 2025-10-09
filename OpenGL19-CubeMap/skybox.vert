#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    // gl_Position在clip space中，经过透视除法转换到NDC，z值仅作为深度
    // 透视除法是在顶点着色器运行之后执行的，将gl_Position的xyz坐标除以w分量
    // 这里将z分量设置为w分量，确保深度值为1.0（远裁剪面），这样天空盒总是在最远处
    // 这样做可以避免天空盒遮挡其他物体
    gl_Position = pos.xyww;
}
