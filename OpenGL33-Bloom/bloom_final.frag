#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer_scene;
uniform sampler2D bloomBlur;
uniform bool enable_bloom;
uniform float exposure;

void main() {
	const float gamma = 2.2;
	vec3 hdrColor = texture(hdrBuffer_scene, TexCoords).rgb;
	vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
	if (enable_bloom) {
		hdrColor += bloomColor;
	}
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}