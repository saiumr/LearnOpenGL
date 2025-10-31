#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// declare an interface block; see 'Advanced GLSL' for what these are.
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool reverse_normal;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));  // get fragment position in world space
    if (reverse_normal) {
        vs_out.Normal = transpose(inverse(mat3(model))) * (-1.0) * aNormal;
    } else {
        vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    }
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}