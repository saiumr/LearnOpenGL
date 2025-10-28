#version 330 core
out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // 转换为 NDC
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far
    // depth = gl_FragCoord.z;  // 使用非线性深度值(近距离变化剧烈，精度高，远距离几乎看不出变化)
    FragColor = vec4(vec3(depth), 1.0);
}