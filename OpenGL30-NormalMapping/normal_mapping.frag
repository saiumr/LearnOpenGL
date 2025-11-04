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

uniform bool normalMapping;

uniform float shininess;

void main() {
	float gamma = 2.2;
	vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;
	vec3 normal =  texture(normalMap, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
	vec3 lightColor = vec3(0.6);

	// ambient
	vec3 ambient = 0.2 * lightColor;

	//diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);  // point light
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

	// specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
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

