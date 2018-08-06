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

struct PntLight
{
	vec3 position;

	vec3 Color;
	float ambient;
	float diffuse;
	float specular;

	float constant;
	float linear;
	float quadratic;
};

struct FlsLight
{
	vec3 position;
	vec3 direction;

	float inCutoff;
	float outCutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

//uniform vec3 viewPos;
uniform Material material;

//#define pnt_lit_count 4
uniform DirLight dirLit;
//uniform PntLight pntLit[pnt_lit_count];
//uniform FlsLight flsLit;
//uniform float ratio[3];

vec4 directional(DirLight );
//vec4 point(PntLight );
//vec4 flash(FlsLight );
uniform sampler2D shadMap[3];
//uniform float gCascadeEndClipSpace[3];
vec3 colo;

vec3 lightDir;

void main()
{
	vec4 outColor;
	outColor += directional(dirLit);
	//for(int i=0;i<pnt_lit_count;++i)
	//{	
	//	colo = de.x * (1.0f-pntLit[i].Color) + de.y * pntLit[i].Color;
	//	outColor += point(pntLit[i]);
	//}
	//outColor += flash(flsLit);
	FragColor = outColor;
}

float linstep(float low,float high,float v)
{
	return clamp((v-low)/(high-low),0.0,1.0);
}

float shadowCalculation(int cascIndex,vec4 fragPosLightSpace)
{
	//vec3 projCord = fragPosLightSpace.xyz/fragPosLightSpace.w;
	//projCord = projCord /2.0f + 0.5;
	//if(projCord.z>1.0f)
	//	return 1.0f;
	//float currentDepth = projCord.z;
	//vec2 moments = texture2D(shadMap[cascIndex],projCord.xy).xy;
	//
	//float p = step(currentDepth,moments.x);
	//float variance = max(moments.y - moments.x*moments.x,0.00002);
	//
	//float d = currentDepth - moments.x;
	//float pMax = linstep(0.4,1.0,variance/(variance+ d*d));
	//
	//return min(max(p,pMax),1.0f);

	vec3 projCord = fragPosLightSpace.xyz/fragPosLightSpace.w;
	projCord = projCord /2.0f + 0.5;
	if(projCord.z>1.0f)
		return 1.0f;
	float currentDepth = projCord.z;
	float fShadowMapValue = texture2D(shadMap[cascIndex],projCord.xy).r;

	//if(fLogMapValue>0.6f && ( fShadowMapValue - currentDepth )>-0.3f)
	//	fShadowMapValue-=0.2f;
	float fShadow = 1.0f;
	fShadow = clamp( exp( fs_in.ratio[cascIndex] * 10.0f * ( fShadowMapValue - currentDepth ) ), 0.0, 1.0 );
	return fShadow;
}

vec4 directional(DirLight light)
{
	//ambient
	vec3 DiffColo = texture(material.diffuse,fs_in.TexCord).rgb;
	//float gamma  = 2.4f;
	//float a = 0.055f;
	//float lim = 0.04045;
	//if(DiffColo.r<=lim)
	//	DiffColo.r /= 12.92;
	//else
	//	DiffColo.r = pow((DiffColo.r + a)/(1+a),gamma);
	//if(DiffColo.g<=lim)
	//	DiffColo.g /= 12.92;
	//else
	//	DiffColo.g = pow((DiffColo.g + a)/(1+a),gamma);
	//if(DiffColo.b<=lim)
	//	DiffColo.b /= 12.92;
	//else
	//	DiffColo.b = pow((DiffColo.b + a)/(1+a),gamma);

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

//vec4 point(PntLight light)
//{
//	//ambient
//	vec3 DiffColo = texture(material.diffuse,fs_in.TexCord).rgb;
//	float gamma  = 2.4f;
//	float a = 0.055f;
//	float lim = 0.04045;
//	if(DiffColo.r<=lim)
//		DiffColo.r /= 12.92;
//	else
//		DiffColo.r = pow((DiffColo.r + a)/(1+a),gamma);
//	if(DiffColo.g<=lim)
//		DiffColo.g /= 12.92;
//	else
//		DiffColo.g = pow((DiffColo.g + a)/(1+a),gamma);
//	if(DiffColo.b<=lim)
//		DiffColo.b /= 12.92;
//	else
//		DiffColo.b = pow((DiffColo.b + a)/(1+a),gamma);
//
//	vec3 ambient = DiffColo * light.ambient*colo;
//	//diffuse
//	lightDir = normalize(light.position - fs_in.fragPos);
//	float diff = max(dot(fs_in.norm,lightDir),0.0f);
//	vec3 diffusion = diff * light.diffuse * colo * DiffColo;
//
//	//specular
//	vec3 viewDir = normalize(viewPos-fs_in.fragPos);
//	vec3 halfWayDir = normalize(lightDir+viewDir);
//	float spec = pow(max(dot(fs_in.norm,halfWayDir),0.0f),material.shininess);
//	//vec3 specular = light.specular * colo * spec * texture(material.specular,fs_in.TexCord).rgb;
//	vec3 specular = light.specular * colo * spec ;
//	//attenution
//	float distance = length(light.position - fs_in.fragPos);
//	//float attenution = 1.0f / (light.constant + light.linear*distance + light.quadratic*(distance * distance));
//	float attenution = 1.0f / (light.linear*distance + light.quadratic*(distance * distance));
//
//	return(vec4((ambient + diffusion + specular) * attenution , 1.0f));
//}
//
//vec4 flash(FlsLight light)
//{
//	//ambient
//	vec3 ambient = texture(material.diffuse,fs_in.TexCord).rgb * light.ambient;
//	//diffuse
//	lightDir = normalize(light.position-fs_in.fragPos);
//	float diff = max(dot(fs_in.norm,lightDir),0.0f);
//	vec3 diffusion = diff * light.diffuse * vec3(texture(material.diffuse,fs_in.TexCord));
//	//specular
//	vec3 viewDir = normalize(viewPos-fs_in.fragPos);
//	vec3 halfWayDir = normalize(lightDir+viewDir);
//	float spec = pow(max(dot(fs_in.norm,halfWayDir),0.0f),material.shininess);
//	vec3 specular = light.specular * spec * vec3(texture(material.specular,fs_in.TexCord));
//	//attenution
//	float distance = length(light.position - fs_in.fragPos);
//	float attenution = 1.0f / (light.constant + light.linear*distance + light.quadratic*(distance * distance));
//	//soft edge spotlight
//	float theta = dot(lightDir,normalize(-light.direction));
//	float epsilon = light.inCutoff - light.outCutoff;
//	float intensity = clamp((theta - light.outCutoff) / epsilon, 0.0f,1.0f);
//	diffusion *= intensity;
//	specular *= intensity;
//
//	return(vec4((ambient + diffusion + specular) * attenution , 1.0f));
//}