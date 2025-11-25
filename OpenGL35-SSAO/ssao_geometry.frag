#version 330 core
// diffrent color attachment in a same frame buffer
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;  // Albedo is diffuse, so this => diffuse + specular and specular texture channel stored in a (of rgba) channel

in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

const float NEAR = 0.1; // Projection matrix's near plane distance
const float FAR = 50.0f; // Projection matrix's far plane distance
// 透视投影会转变为非线性深度，此处将非线性深度转换为线性深度
float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));	
}

void main() {
	gPositionDepth.xyz = fs_in.FragPos;
	gPositionDepth.w = LinearizeDepth(gl_FragCoord.z);
	gNormal = normalize(fs_in.Normal);
	//gAlbedoSpec.rgb = texture(texture_diffuse1, fs_in.TexCoords).rgb;
	//gAlbedoSpec.a = texture(texture_specular1, fs_in.TexCoords).r;
	gAlbedoSpec = vec4(0.95, 0.95, 0.95, 1.0);
}
