#version 330 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices
{
	// The order of member variables is crucial
	mat4 projection;
	mat4 view;
};
uniform	mat4 model;  // model is not part of ubo and serves as a control group

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
