#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// 在顶点着色器中，利用TBN的逆矩阵，将所有向量转换到切线空间中
// 顶点着色器运行次数比片段着色器少，相比于直观的将切线空间中的法线转换到世界空间，节省了不少计算资源
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
	vs_out.FragPos = vec3(model * vec4(position, 1.0));
	vs_out.TexCoords = texCoords;
	gl_Position = projection * view * model * vec4(position, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal);
    // 共享顶点多的时候TBN可能就不是正交的了，我们可以用施密特正交化计算出副切线B（N和T、B一定正交，N垂直于切面）
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));  // 正交矩阵的转置  <=>  矩阵的逆
    // 世界空间 -> 切线空间
    vs_out.TangentLightPos = TBN * lightPos;         // <=> T、B、N分别与lightPos点乘（dot）组合而成的向量；有些代码可能会直接点乘计算
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
}
