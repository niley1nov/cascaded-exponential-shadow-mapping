#version 420 core

out vec4 FragColor;

in VS_OUT
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
	float artifact;
	float cascade;
}fs_in;

struct Material
{
	sampler2D diffuse;
	//sampler2D specular;
	float shininess;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform DirLight dirLit;
vec4 directional(DirLight );
uniform sampler2D shadMap[3];
uniform sampler2D logMap[3];
vec3 colo;

vec3 lightDir;

void main()
{
	vec4 outColor;
	outColor += directional(dirLit);
	FragColor = outColor;
}

float linstep(float low,float high,float v)
{
	return clamp((v-low)/(high-low),0.0,1.0);
}

float shadowCalculation(int cascIndex,vec4 fragPosLightSpace)
{
	//there are 3 cascaded shadow maps stored in array shadMap and indexed by cascIndex. 
	// fragPosLightSpace is fragment position in light space.
	vec3 projCord = fragPosLightSpace.xyz/fragPosLightSpace.w;
	projCord = projCord /2.0f + 0.5;
	if(projCord.z>1.0f)
		return 1.0f;
	float ratio = fs_in.ratio[cascIndex] / fs_in.ratio[0];
	float currentDepth = ratio *projCord.z;
	float fShadowMapValue = ratio *texture2D(shadMap[cascIndex],projCord.xy).r;
	float fLogMapValue = texture2D(logMap[cascIndex],projCord.xy).r;
	float diff1 = currentDepth - fShadowMapValue;
	float a = 0.1f;
	if(fLogMapValue>0.3f && diff1<a && fs_in.artifact>0.5f)
	{
		float mul1 = clamp(fLogMapValue,0.3f,0.7f)-0.3f;
		mul1 *= 2.5f;
		currentDepth+=((a -diff1)*mul1);
	}
	float fShadow = 1.0f;
	fShadow = clamp( exp( 10.0f * ( fShadowMapValue - currentDepth ) ), 0.0, 1.0 );
	return fShadow;
}

vec4 directional(DirLight light)
{
	//ambient
	vec3 DiffColo = texture(material.diffuse,fs_in.TexCord).rgb;

	vec3 ambient = DiffColo * light.ambient;
	//diffuse
	lightDir = normalize(vec3(-light.direction));
	float diff = max(dot(fs_in.norm,lightDir),0.0f);
	vec3 diffusion = diff * light.diffuse * DiffColo;
	//specular
	vec3 viewDir = normalize(fs_in.viewPos-fs_in.fragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(fs_in.norm,halfWayDir),0.0f),material.shininess);
	vec3 specular = light.specular * spec;
	
	float shadow = 0.0f;
	vec3 debugColo[3] = {vec3(1.0f,0.5f,0.5f),vec3(0.5f,1.0f,0.5f),vec3(0.5f,0.5f,1.0f)};
	int chose = 0;
	for(int i=0;i<3;++i)
	{
		if(fs_in.clipSpacePosZ<=fs_in.gCascadeEndClipSpace[i])
		{
			shadow = shadowCalculation(i,fs_in.FragPosLightSpace[i]);
			chose = i;
			break;
		}
	}
	if(fs_in.cascade>0.5f)
		return(vec4(debugColo[chose]*(ambient+(diffusion+specular)*(shadow)),1.0f));
	return(vec4(fs_in.dirColor*(ambient+(diffusion+specular)*(shadow)),1.0f));
}