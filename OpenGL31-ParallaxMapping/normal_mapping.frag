#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform bool enable_paralax_mapping;
uniform float height_scale;

uniform float shininess;

vec2 ParalaxMapping_Basic(vec2 texCoords, vec3 viewDir) {
    float height = texture(depthMap, texCoords).r;
    vec2  p_offset = (viewDir.xy / viewDir.z) * (height * height_scale);
    
    return texCoords - p_offset;
}

// 当视角很倾斜（看到的高度差过大）会出现失真，利用陡峭视差映射解决
// 将深度均匀分层，找到深度值小于层级深度的那一层（不被覆盖），取其纹理UV坐标
vec2 ParalaxMapping_Steep(vec2 texCoords, vec3 viewDir) {
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));  // 根据视角倾斜程度动态设置层数
    float layerDepth = 1.0 / 10;
    float currentLayerDepth = 0.0;
    vec2 p_offset = viewDir.xy * height_scale;
    vec2 deltaTexCoords = p_offset / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

    while (currentDepthMapValue > currentLayerDepth) {
        currentLayerDepth += layerDepth;
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;
    }

    return currentTexCoords;
}

void main() {
	float gamma = 2.2;
    vec2 tex_coords = fs_in.TexCoords;
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    if (enable_paralax_mapping) {
        tex_coords = ParalaxMapping_Steep(tex_coords, viewDir);
    }

    if(tex_coords.x > 1.0 || tex_coords.y > 1.0 || tex_coords.x < 0.0 || tex_coords.y < 0.0)
        discard;

	vec3 color = texture(diffuseMap, tex_coords).rgb;
	vec3 normal =  texture(normalMap, tex_coords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
	vec3 lightColor = vec3(0.6);

	// ambient
	vec3 ambient = 0.1 * lightColor;

	//diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);  // point light
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

	// specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    // Blinn-Phong lighting model
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = lightColor * spec;

	// final color
	FragColor = vec4((ambient + diffuse + specular) * color, 1.0);

	// gamma encode
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}

