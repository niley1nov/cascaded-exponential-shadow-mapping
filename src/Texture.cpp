#pragma once
#include"stb_image.h"
#include"Texture.h"

Texture::Texture(const std::string & location,const GLenum& wrap)
{
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	stbi_set_flip_vertically_on_load(true);
	m_LocalBuffer = stbi_load(location.c_str(), &width, &height, &nrChannels, 4);
	if (m_LocalBuffer)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}
	else
	{
		std::cout << "Failed to load Texture!" << std::endl;
	}
	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

Texture::Texture(const std::vector<std::string>& faces)
{
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); ++i)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		}
		else
			std::cout << "ERROR:: Cube Texturefailed to load at path " << faces[i] << "." << std::endl;
		stbi_image_free(data);
	}
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int count,GLenum type)
{
	GLCall(glActiveTexture(GL_TEXTURE0 + count));
	GLCall(glBindTexture(type, m_RendererID));
}

void Texture::Unbind()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
