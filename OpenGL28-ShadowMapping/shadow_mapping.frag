#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuse_texture;
uniform sampler2D depthMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float shininess;

float ShadowCalculation(vec4 fragPosLightSpace, float bias) {
    // 下面这行透视除法只在透视投影有用，正交投影w=1不会变化
    // 保留下来为了将平行光（正交投影）改为点光源（透视投影）也能使用
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;  // <=> (projCoords + 1)*0.5, transform it to NDC
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    // float closestDepth = texture(depthMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    // PCF  百分比渐进滤波(percentage-closer filtering)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            // 对深度贴图多次采样邻近深度取平均值抗锯齿，这里是 3x3 共取 9 个像素点最近深度
            // 当然，也可以直接增加深度贴图分辨率抗锯齿，PCF是出于性能考虑的，而且有多种方法，并不是只能取平均值
            // 近距离可以观察到阴影边缘变得柔和了
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // 光锥之外深度值大于1，让它们阴影度为0模拟太阳更真实
    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

void main()
{           
    float gamma = 2.2;
    vec3 color = texture(diffuse_texture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.2 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    // Blinn-Phong lighting model
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = lightColor * spec; // assuming bright white light color

    // sun light no attenuation
    /*
    float max_distance = 1.5;
    float distance = length(lightPos - fs_in.FragPos);
    float attenuation = 1.0 / (distance * distance);

    diffuse *= attenuation;
    specular *= attenuation;
    */

    // calculate shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);
    // 环境光不受阴影影响，但倘若片段在阴影中，将失去漫反射光和镜面光
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);

    // gamma encode
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}