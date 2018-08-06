#pragma once
#include"Renderer.h"
#include<vector>

struct LayoutElement
{
	unsigned int type;
	int count;
	unsigned char norm;
};

class VertexBufferLayout
{
private:
	std::vector<LayoutElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		:m_Stride(0) {}
	~VertexBufferLayout() {}
	
	template <typename T>
	void push(int count)
	{
		ASSERT(false);
	}
	
	template<>
	void push<float>(int count)
	{
		m_Elements.push_back({ GL_FLOAT,count,GL_FALSE });
		m_Stride += count * sizeof(GLfloat);
	}

	template<>
	void push<unsigned int>(int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT,count,GL_FALSE });
		m_Stride += count * sizeof(GLuint);
	}

	template<>
	void push<unsigned char>(int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE,count,GL_TRUE });
		m_Stride += count * sizeof(GLubyte);
	}

	inline const std::vector<LayoutElement>& GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};