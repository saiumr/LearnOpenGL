#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;       // firstly, from local space to world space
uniform mat4 view;        // secondly, from world space to view space
uniform mat4 projection;  // thirdly, from view space to clip space

void main()
{
	gl_Position = projection * view * model * vec4( aPos.x, aPos.y, aPos.z, 1.0);
	TexCoord = aTexCoord;
}
