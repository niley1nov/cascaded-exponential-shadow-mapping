#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCord;
layout(location = 3) in mat4 aModelMats;

struct uniBuf
{
	mat4 projView;
	mat4 lightSpaceMat[3];
	vec3 m_viewPos;
	float pad1;
	vec3 m_ratio;
	float m_cascade;
	vec3 m_gCascadeEndClipSpace;
	float pad3;
	vec3 m_dirColor;
	float pad4;
};



layout(std140,binding = 1) uniform Matrices
{
	uniBuf buf1;
};

out VS_OUT
{
	vec3 norm;
	vec3 fragPos;
	vec3 viewPos;
	vec3 dirColor;
	vec2 TexCord;
	vec4 FragPosLightSpace[3];
	float clipSpacePosZ;
	float ratio[3];
	float gCascadeEndClipSpace[3];
	float cascade;
}vs_out;

void main()
{
	gl_Position = buf1.projView * aModelMats * vec4(aPos,1.0f);
	vs_out.fragPos = vec3(aModelMats * vec4(aPos,1.0f));
	vs_out.norm = normalize(mat3(transpose(inverse(aModelMats))) * aNormal);
	vs_out.TexCord = aTexCord;
	vs_out.clipSpacePosZ = gl_Position.z;
	vs_out.dirColor = buf1.m_dirColor;
	for(int i=0;i<3;++i)
	{
		vs_out.FragPosLightSpace[i] = buf1.lightSpaceMat[i] * vec4(vs_out.fragPos,1.0f);
	}
	vs_out.gCascadeEndClipSpace[0] = buf1.m_gCascadeEndClipSpace.x;
	vs_out.gCascadeEndClipSpace[1] = buf1.m_gCascadeEndClipSpace.y;
	vs_out.gCascadeEndClipSpace[2] = buf1.m_gCascadeEndClipSpace.z;
	vs_out.ratio[0] = buf1.m_ratio.x;
	vs_out.ratio[1] = buf1.m_ratio.y;
	vs_out.ratio[2] = buf1.m_ratio.z;
	vs_out.viewPos = buf1.m_viewPos;
	vs_out.cascade = buf1.m_cascade;
}