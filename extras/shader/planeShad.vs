#version 420 core
layout(location = 0) in vec2 aPos;

layout(std140,binding = 0) uniform Matrices
{
	mat4 lightSpaceMat;
};

uniform mat4 model;

void main()
{
	gl_Position = lightSpaceMat * model * vec4(aPos,0.0f,1.0f);
}