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
    // ��������͸�ӳ���ֻ��͸��ͶӰ���ã�����ͶӰw=1����仯
    // ��������Ϊ�˽�ƽ�й⣨����ͶӰ����Ϊ���Դ��͸��ͶӰ��Ҳ��ʹ��
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

    // PCF  �ٷֱȽ����˲�(percentage-closer filtering)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            // �������ͼ��β����ڽ����ȡƽ��ֵ����ݣ������� 3x3 ��ȡ 9 �����ص�������
            // ��Ȼ��Ҳ����ֱ�����������ͼ�ֱ��ʿ���ݣ�PCF�ǳ������ܿ��ǵģ������ж��ַ�����������ֻ��ȡƽ��ֵ
            // ��������Թ۲쵽��Ӱ��Ե��������
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // ��׶֮�����ֵ����1����������Ӱ��Ϊ0ģ��̫������ʵ
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
    // �����ⲻ����ӰӰ�죬������Ƭ������Ӱ�У���ʧȥ�������;����
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);

    // gamma encode
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}