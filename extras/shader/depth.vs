#version 420 core
layout(location = 0)in vec3 aPos;
layout(Location = 1)in mat4 aMats;

layout(std140,binding = 0) uniform Matrices
{
	mat4 lightSpaceMat;
};
uniform vec2 m_stencil;
out vec2 stencil;

void main()
{	
	gl_Position = lightSpaceMat * aMats * vec4(aPos,1.0f);
	stencil = m_stencil;
}