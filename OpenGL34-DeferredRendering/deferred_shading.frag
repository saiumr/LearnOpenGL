#version 330 core
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// 单个光源参数（不再是数组）
struct Light {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};
uniform Light light;

uniform vec3 viewPos;

void main() {
    vec2 texCoords = gl_FragCoord.xy / textureSize(gPosition, 0);
    vec3 FragPos = texture(gPosition, texCoords).rgb;
    vec3 Normal = texture(gNormal, texCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, texCoords).rgb;
    float Specular = texture(gAlbedoSpec, texCoords).a;

    // 2. 跳过背景像素（没有物体的地方，不计算光照）
    if (length(FragPos) < 0.1f) {
        FragColor = vec4(0.0f);
        return;
    }

    // 3. 计算光照（没有if判断！因为球体已经限制了像素范围）
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(light.Position - FragPos);
    
    // 漫反射
    float diff = max(dot(Normal, lightDir), 0.0f);
    vec3 diffuse = diff * Diffuse * light.Color;
    
    // 高光（Blinn-Phong）
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0f), 16.0f);
    vec3 specular = spec * Specular * light.Color;
    
    // 衰减（和原来一样）
    float distance = length(light.Position - FragPos);
    float attenuation = 1.0f / (1.0f + light.Linear * distance + light.Quadratic * distance * distance);
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    // 输出当前光源的光照贡献（后续会通过混合累加）
    FragColor = vec4(diffuse + specular, 1.0f);
}