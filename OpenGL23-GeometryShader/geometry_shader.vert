#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out VS_OUT {
    vec3 color;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.color = aColor;
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, 0.0, 1.0);   // draw on Z plane
}