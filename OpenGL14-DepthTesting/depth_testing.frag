#version 330 core
out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // ת��Ϊ NDC
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // Ϊ����ʾ���� far
    // depth = gl_FragCoord.z;  // ʹ�÷��������ֵ(������仯���ң����ȸߣ�Զ���뼸���������仯)
    FragColor = vec4(vec3(depth), 1.0);
}