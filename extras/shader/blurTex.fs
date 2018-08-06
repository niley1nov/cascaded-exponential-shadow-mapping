#version 420 core

out vec4 FragColor;
uniform sampler2D Texture1;
in VS_OUT
{
	vec2 cascNum;
	vec2 TexCord;
	vec2 blurmultiplyVec; //verticle 0,1 //horizontal 1,0
}fs_in;

float sigma = 4.0;
const float blurSize = 1.0 / (1024.0*fs_in.cascNum.x);
const float pi = 3.14159265;
const float numBlurPixelPerSide = 4.0;

void main()
{
	vec3 increamentalGaussian;
	increamentalGaussian.x = 1.0/(sqrt(2.0*pi)*sigma);
	increamentalGaussian.y= exp(-0.5/(sigma*sigma));
	increamentalGaussian.z = increamentalGaussian.y * increamentalGaussian.y;
	
	vec4 avgValue = vec4(0.0);
	float coefficientSum = 0.0;
	
	avgValue += texture2D(Texture1,fs_in.TexCord.st)*increamentalGaussian.x;
	coefficientSum += increamentalGaussian.x;
	increamentalGaussian.xy *= increamentalGaussian.yz;
	
	for(float i=1.0;i<= numBlurPixelPerSide;i++)
	{
		avgValue += texture2D(Texture1,fs_in.TexCord.st -i*blurSize*fs_in.blurmultiplyVec)*increamentalGaussian.x;
		avgValue += texture2D(Texture1,fs_in.TexCord.st +i*blurSize*fs_in.blurmultiplyVec)*increamentalGaussian.x;
		coefficientSum+= 2.0*increamentalGaussian.x;
		increamentalGaussian.xy *= increamentalGaussian.yz;
	}
	FragColor = avgValue/coefficientSum;
}
