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

// 基本视差映射
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
    vec2 p_offset = viewDir.xy / viewDir.z * height_scale;
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

// 陡峭视差映射可能会看到明显的分层，利用视差遮挡映射（或者比较吃性能的浮雕(relief)视差映射解决），扩展陡峭视差映射算法即可
// 我们找到符合条件的那一层之后，取那层和上一层在视差题图中的深度进行插值，这样获得的结果更接近真实记录的深度
// 同理，偏移后的纹理坐标也就是两个坐标插值
vec2 ParalaxMapping_Occlusion(vec2 texCoords, vec3 viewDir) {
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
    float layerDepth = 1.0 / 10;
    float currentLayerDepth = 0.0;
    vec2 p_offset = viewDir.xy / viewDir.z * height_scale;
    vec2 deltaTexCoords = p_offset / numLayers;

    vec2 currentTexCoords = texCoords;
    vec2 pre_currentTexCoords = currentTexCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

    while (currentDepthMapValue > currentLayerDepth) {
        currentLayerDepth += layerDepth;
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main() {
	float gamma = 2.2;
    vec2 tex_coords = fs_in.TexCoords;
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    if (enable_paralax_mapping) {
        tex_coords = ParalaxMapping_Occlusion(tex_coords, viewDir);
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

