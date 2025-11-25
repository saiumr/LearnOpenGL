#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;
uniform bool ssao_enabled;


// 单个光源参数
struct Light {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};
uniform Light light;

void main() {
    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    vec3 viewDir = normalize(-FragPos);  // viewPos is (0,0,0)
    vec3 lightDir = normalize(light.Position - FragPos);  // 这里的postion应该是摄像机空间的坐标，FragPos是摄像机空间坐标
    
    // ambient
    vec3 ambient = vec3(0.3);
    if (ssao_enabled) {
        ambient *= AmbientOcclusion;
    }
    vec3 lighting = ambient;

    // diffuse
    float diff = max(dot(Normal, lightDir), 0.0f);
    vec3 diffuse = diff * Diffuse * light.Color;
    
    // specular(Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0f), 8.0f);
    vec3 specular = spec * Specular * light.Color;
    
    // attenuation
    float distance = length(light.Position - FragPos);
    float attenuation = 1.0f / (1.0f + light.Linear * distance + light.Quadratic * distance * distance);
    
    diffuse *= attenuation;
    specular *= attenuation;

    // final lighting contribution
    lighting += diffuse + specular;
    
    FragColor = vec4(lighting, 1.0f);
}