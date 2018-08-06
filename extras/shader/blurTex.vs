#version 420 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCord;

uniform vec2 xy;
uniform float m_cascNum;
out VS_OUT
{
	vec2 cascNum;
	vec2 TexCord;
	vec2 blurmultiplyVec;
}vs_out;


void main()
{
	vs_out.blurmultiplyVec = xy;
	vs_out.TexCord = aTexCord;
	vs_out.cascNum = vec2(m_cascNum,0.0f);
	gl_Position = vec4(aPos.xy,0.0f,1.0f);
}