#version 420 core

out vec4 FragColor;
uniform sampler2D Texture1;
in vec2 TexCord;

void main()
{
	FragColor = texture(Texture1,TexCord);
	FragColor = vec4(vec3(FragColor.r),1.0f);
}