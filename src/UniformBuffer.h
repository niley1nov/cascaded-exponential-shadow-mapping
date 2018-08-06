#pragma once
#include"Renderer.h"
#include<vector>

class UniformBuffer
{
	unsigned int m_RendererID;
	std::vector<unsigned int> slots;
	unsigned int calcSize(unsigned int index)
	{
		unsigned int size = 0;
		for (unsigned int i = 0; i < index; ++i)
		{
			size += slots[i];
		}
		return size;
	}
public:
	UniformBuffer() {}
	~UniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
	void push(unsigned int count,unsigned int size)
	{
		for (unsigned int i = 0; i < count; ++i)
			slots.push_back(size);
	}
	void BuildBuffer(unsigned int index, GLenum usage)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		unsigned int size = calcSize(slots.size());
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, usage);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, index, m_RendererID, 0, size);
	}
	void fillData(const unsigned int& slotNum, const void* data)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, calcSize(slotNum), slots[slotNum], data);
	}
	void Bind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
	}
	void Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
};