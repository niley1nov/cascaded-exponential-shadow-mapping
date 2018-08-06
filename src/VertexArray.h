#pragma once
#include"Renderer.h"
#include"IndexBuffer.h"
#include"VertexBuffer.h"
#include"VertexBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID;
	unsigned int m_index;
public:
	VertexArray() :
		m_index(0)
	{
		GLCall(glGenVertexArrays(1, &m_RendererID));
	}
	~VertexArray() 
	{
		GLCall(glDeleteVertexArrays(1, &m_RendererID));
	}
	void AddBuffNLay(const VertexBuffer& vb, const VertexBufferLayout& vl, const IndexBuffer& ib)
	{
		Bind();
		vb.Bind();
		ib.Bind();
		const auto& Elements = vl.GetElements();
		unsigned int ptr = 0;
		for (unsigned int i = 0; i < Elements.size(); ++i)
		{
			const auto& element = Elements[i];
			GLCall(glEnableVertexAttribArray(i));
			GLCall(glVertexAttribPointer(i, element.count, element.type, element.norm, vl.GetStride(), (void*)ptr));
			ptr += element.count * sizeof(element.type);
		}
	}
	void AddBuffNLay(const VertexBuffer& vb, const VertexBufferLayout& vl)
	{
		Bind();
		const auto& Elements = vl.GetElements();
		unsigned int ptr = 0;
		bool flag = false;
		if (m_index != 0)
		{
			flag = true;
		}
		vb.Bind();
		for (unsigned int i = 0; i < Elements.size(); ++i)
		{
			const auto& element = Elements[i];
			GLCall(glEnableVertexAttribArray(m_index));
			GLCall(glVertexAttribPointer(m_index, element.count, element.type, element.norm, vl.GetStride(), (void*)ptr));
			ptr += element.count * sizeof(element.type);
			if(flag)
				glVertexAttribDivisor(m_index, 1);
			++m_index;
		}
	}
	void AddBuffNLaySub(const VertexBuffer& vb, const VertexBufferLayout& vl)
	{
		Bind();
		const auto& Elements = vl.GetElements();
		unsigned int ptr = 0;
		bool flag = false;
		if (m_index != 0)
		{
			flag = true;
		}
		vb.Bind();
		for (unsigned int i = 0; i < Elements.size(); ++i)
		{
			const auto& element = Elements[i];
			GLCall(glEnableVertexAttribArray(m_index));
			GLCall(glVertexAttribPointer(m_index, element.count, element.type, element.norm, element.count * sizeof(element.type), (void*)ptr));
			ptr += (sizeof(element.type)* vb.GetWrdNum());
			if (flag)
				glVertexAttribDivisor(m_index, 1);
			++m_index;
		}
	}
	unsigned int GetID()
	{
		return m_RendererID;
	}
	void Bind()
	{
		GLCall(glBindVertexArray(m_RendererID));
	}
	void Unbind()
	{
		GLCall(glBindVertexArray(0));
	}
};