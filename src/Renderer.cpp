#include "Renderer.h"
#include"Shader_s.h"
#include"VertexArray.h"
#include"IndexBuffer.h"
#include"Texture.h"

std::string convert(int a)
{
	char x[9];
	sprintf_s(x, "%x", a);
	std::string zero = "0";
	std::string str = x;
	int len = str.size();
	for (; len < 4; len++)
	{
		str = zero + str;
	}
	return str;
}

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char * function, const char * file, int LineNo)
{
	while (GLenum error = glGetError())
	{
		std::string err = convert(error);
		std::ifstream glew("src/Error.list");
		if (glew.fail())
		{
			std::cout << "Error file didn't open" << std::endl;
			glew.clear();
		}
		std::string line;
		while (getline(glew, line))
		{
			if (line.find(err.c_str()) != std::string::npos)
			{
				std::cout << line << std::endl;
			}
		}
		std::cout << function << " " << file << " : " << LineNo << std::endl;
		return false;
	}
	return true;
}

void Renderer::Draw(GLenum mode,VertexArray& va,IndexBuffer& ib,Shader& shader)
{
	shader.use();
	va.Bind();
	GLCall(glDrawElements(mode, ib.GetCount(), GL_UNSIGNED_INT,0));
}

void Renderer::Draw(GLenum mode,VertexArray& va, Shader& shader,const int& count,int first)
{
	shader.use();
	va.Bind();
	GLCall(glDrawArrays(mode, first, count));
}

void Renderer::DrawInstanced(GLenum mode, VertexArray & va, IndexBuffer & ib, Shader & shader, unsigned int pirimeCount)
{
	shader.use();
	va.Bind();
	GLCall(glDrawElementsInstanced(mode, ib.GetCount(), GL_UNSIGNED_INT, 0, pirimeCount));
}

void Renderer::DrawInstanced(GLenum mode, VertexArray & va, Shader & shader, const int & count, unsigned int primeCount, int first)
{
	shader.use();
	va.Bind();
	GLCall(glDrawArraysInstanced(mode, first, count, primeCount));
}

void Renderer::clearColorNDepth(const glm::vec4 & colorValue,const bool& depth)
{
	GLCall(glClearColor(colorValue.x,colorValue.y,colorValue.z,colorValue.w));
	if (depth)
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
	else
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	}
}

void Renderer::PauseMenu(GLFWwindow* window)
{
	if (status == false)
	{
		status = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPosCallback(window, mouse_pause);
		glfwSetScrollCallback(window, NULL);
		pause = true;
	}
	else
	{
		status = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		pause = false;
	}
	tilde = true;
}

bool Renderer::tilde = false;
bool Renderer::status = false;


