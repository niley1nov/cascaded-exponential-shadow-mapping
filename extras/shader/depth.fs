#version 420 core
out vec4 FragColor;
in vec2 stencil;

void main()
{
	if(stencil.x >0.5f)
	{
		float depth = gl_FragCoord.z;
		FragColor = vec4(depth,depth,depth,1.0f);
	}
	else if(stencil.x == 0.0f)
	{
		FragColor = vec4(1.0f);
	}
}