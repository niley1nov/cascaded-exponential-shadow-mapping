#pragma once
#ifndef SHADER_S
#define SHADER_S

#include"Renderer.h"
#include<map>

class Shader
{
private:
	unsigned int m_RendererID;
	float mul;
	std::map<GLenum, std::string> shaderName;
	std::map<GLenum, unsigned int> ShaderIDs;
	void gl_checkCompileErrors(unsigned int id,const std::string& name)
	{
		if (name != "program")
		{
			int result;
			GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
			if (result == GL_FALSE)
			{
				int length;
				GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
				char* message = (char*)alloca(length*sizeof(char));
				GLCall(glGetShaderInfoLog(id, length, &length, message));
				std::cout << "Failed to compile " << name << " shader!" << std::endl;
				std::cout << message << std::endl;
				GLCall(glDeleteShader(id));
			}
		}
		else
		{
			int result;
			GLCall(glGetProgramiv(id, GL_LINK_STATUS, &result));
			if (result == GL_FALSE)
			{
				int length;
				GLCall(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length));
				char* message = (char*)alloca(length * sizeof(char));
				GLCall(glGetProgramInfoLog(id, length, &length, message));
				std::cout << "Failed to link program!" << std::endl;
				std::cout << message << std::endl;
				GLCall(glDeleteProgram(id));
			}
		}
	}
public:
	Shader()
		:mul(0.0f)
	{
		shaderName.insert(std::pair<GLenum, std::string>(GL_VERTEX_SHADER, "vertex"));
		shaderName.insert(std::pair<GLenum, std::string>(GL_TESS_CONTROL_SHADER, "tess_control"));
		shaderName.insert(std::pair<GLenum, std::string>(GL_TESS_EVALUATION_SHADER, "tess_eval"));
		shaderName.insert(std::pair<GLenum, std::string>(GL_GEOMETRY_SHADER, "geometry"));
		shaderName.insert(std::pair<GLenum, std::string>(GL_FRAGMENT_SHADER, "fragment"));
	}
	void CopyShader(unsigned int ID)
	{
		m_RendererID = ID;
	}
	void push(GLenum ShaderType, const std::string& ShaderPath)
	{
		std::fstream ShaderFile;
		std::string ShaderCode;

		ShaderFile.exceptions(std::istream::failbit | std::istream::badbit);
		try
		{
			ShaderFile.open(ShaderPath.c_str(), std::ios::in);
			std::stringstream ShaderStream;
			ShaderStream << ShaderFile.rdbuf();
			ShaderCode = ShaderStream.str();
			ShaderFile.close();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::Can not open shader file" << std::endl;
		}
		const char* Source = ShaderCode.c_str();
		unsigned int ShaderID;
		ShaderID = glCreateShader(ShaderType);
		GLCall(glShaderSource(ShaderID, 1, &Source, NULL));
		GLCall(glCompileShader(ShaderID));
		gl_checkCompileErrors(ShaderID, shaderName[ShaderType]);
		ShaderIDs.insert(std::pair<GLenum, unsigned int>(ShaderType, ShaderID));
	}
	void buildShader()
	{
		m_RendererID = glCreateProgram();
		std::map<GLenum, unsigned int> ::iterator it = ShaderIDs.begin();
		while (it != ShaderIDs.end())
		{
			GLCall(glAttachShader(m_RendererID, it->second));
			it++;
		}
		GLCall(glLinkProgram(m_RendererID));
		GLCall(glValidateProgram(m_RendererID));
		gl_checkCompileErrors(m_RendererID, "program");
		while (it != ShaderIDs.end())
		{
			GLCall(glDeleteShader(it->second));
		}
	}
	void incMul()
	{
		if (mul < 1.0f)
			mul += 0.01f;
	}
	void decMul()
	{
		if (mul > 0.0f)
			mul -= 0.01f;
	}
	float getMul()
	{
		return mul;
	}
	unsigned int GetID()
	{
		return m_RendererID;
	}
	void use()
	{
		GLCall(glUseProgram(m_RendererID));
	}
	void setBool(const std::string& name, bool value) const
	{
		GLCall(glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), (int)value));
	}
	void setInt(const std::string& name, int value) const
	{
		GLCall(glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value));
	}
	void setFloat(const std::string& name, float value) const
	{
		GLCall(glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value));
	}
	void setMat4(const std::string& name, const float *matrix) const
	{
		GLCall(glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, matrix));
	}
	void setVec2(const std::string& name, const glm::vec2& vector) const
	{
		GLCall(glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &vector[0]));
	}
	void setVec3(const std::string& name,const glm::vec3& vector) const
	{
		GLCall(glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &vector[0]));
	}
	void setVec4(const std::string& name, const glm::vec4& vector) const
	{
		GLCall(glUniform4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &vector[0]));
	}
	void setBool(const unsigned int &index, bool value) const
	{
		GLCall(glUniform1i(index, (int)value));
	}
	void setInt(const unsigned int &index, int value) const
	{
		GLCall(glUniform1i(index, value));
	}
	void setFloat(const unsigned int &index, float value) const
	{
		GLCall(glUniform1f(index, value));
	}
	void setMat4(const unsigned int &index, const float *matrix) const
	{
		GLCall(glUniformMatrix4fv(index, 1, GL_FALSE, matrix));
	}
	void setVec2(const unsigned int &index, const glm::vec2& vector) const
	{
		GLCall(glUniform2fv(index, 1, &vector[0]));
	}
	void setVec3(const unsigned int &index, const glm::vec3& vector) const
	{
		GLCall(glUniform3fv(index, 1, &vector[0]));
	}
	void setVec4(const unsigned int &index, const glm::vec4& vector) const
	{
		GLCall(glUniform4fv(index, 1, &vector[0]));
	}
	void setMode(const int& mode)
	{
		setBool("Direc", !bool(mode));
		setBool("Pnt", !bool(mode - 1));
		setBool("Fls", !bool(mode - 2));
	}
	void dirStruct_set(glm::vec3* SET)
	{
		setVec3("dirLit.direction", SET[0]);
		setVec3("dirLit.ambient", SET[1]);
		setVec3("dirLit.diffuse", SET[2]);
		setVec3("dirLit.specular", SET[3]);
	}
	void pntStruct_set(glm::vec3* SET,int i)
	{
		std::string num = std::to_string(i);
		setVec3("pntLit["+num+"].position", SET[0]);
		setVec3("pntLit["+num+"].Color", SET[1]);
		setFloat("pntLit["+num+"].ambient", SET[2].x);
		setFloat("pntLit["+num+"].diffuse", SET[2].y);
		setFloat("pntLit["+num+"].specular", SET[2].z);
		setFloat("pntLit["+num+"].constant", SET[3].x);
		setFloat("pntLit["+num+"].linear", SET[3].y);
		setFloat("pntLit["+num+"].quadratic", SET[3].z);
	}
	void flsStruct_set(glm::vec3* SET)
	{
		setFloat("flsLit.inCutoff", SET[0].x);
		setFloat("flsLit.outCutoff", SET[0].y);
		setVec3("flsLit.ambient", SET[1]);
		setVec3("flsLit.diffuse", SET[2]);
		setVec3("flsLit.specular", SET[3]);
		setFloat("flsLit.constant", SET[4].x);
		setFloat("flsLit.linear", SET[4].y);
		setFloat("flsLit.quadratic", SET[4].z);
	}
};
#endif