#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform int select_num;

void main() {
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;

	vec3 result = Diffuse;
	if ( select_num == 0 ) {
		result = FragPos;
	} else if ( select_num == 1 ) {
		result = Normal;
	} else if ( select_num == 2 ) {
		result = Diffuse;
	} else {
		result = vec3(Specular);
	}

	FragColor = vec4(result, 1.0);
}
