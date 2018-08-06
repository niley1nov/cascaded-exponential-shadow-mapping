#pragma once
#ifndef RENDER_H
#define RENDER_H

#include "GLEW/glew.h"
#include"GLFW\glfw3.h"
#include"glm\glm.hpp"
#include"glm\gtc\matrix_transform.hpp"
#include"glm\gtc\type_ptr.hpp"
#include"Camera.h"
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
class VertexArray;
class IndexBuffer;
class Shader;
class Texture;

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError(); \
	x;\
	ASSERT(GLLogCall(#x,__FILE__,__LINE__))

std::string convert(int a);
void GLClearError();
bool GLLogCall(const char* function, const char* file, int LineNo);
void mouse_callback(GLFWwindow*, double, double);
void mouse_pause(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
void processInput(GLFWwindow*);

class Renderer:private Camera
{
public:
	static bool status;
	static bool tilde;
	void Draw(GLenum mode,VertexArray& va,IndexBuffer& ib,Shader& shader);
	void Draw(GLenum mode,VertexArray& va, Shader& shader,const int& count,int first = 0);
	void DrawInstanced(GLenum mode, VertexArray& va, IndexBuffer& ib, Shader& shader,unsigned int pirimeCount);
	void DrawInstanced(GLenum mode, VertexArray& va, Shader& shader, const int& count, unsigned int primeCount, int first = 0);
	void clearColorNDepth(const glm::vec4& colorValue,const bool& depth);
	static void PauseMenu(GLFWwindow* window);
};

#endif