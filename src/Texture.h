#pragma once
#include"Renderer.h"
#include<vector>
#include<string>

class Texture
{
private: 
	unsigned int m_RendererID;
	int width, height, nrChannels;
	unsigned char* m_LocalBuffer;
public:
	Texture(const std::string& location, const GLenum& wrap);
	Texture(const std::vector<std::string>&);
	~Texture();
	void Bind(unsigned int,GLenum type);
	void Unbind();
};