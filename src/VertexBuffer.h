#pragma once
#include"Renderer.h"
#include<vector>
#include<string>
#include<fstream>

struct aData
{
	float *vData[3];
	unsigned int dSize[3];
	unsigned int num_wrds;
	aData()
	{
		for (unsigned int i = 0; i < 3; ++i)
		{
			dSize[i] = 0;
		}
		num_wrds = 0;
	}
};

class VertexBuffer
{
private:
	unsigned int m_RendererID;
	//float *aData[3];// , *aTex, *aNorm;
	aData dObj;
	//unsigned int num_wrds;
	bool priority[3];
	std::vector<float> data[3];
	//std::vector<float> norm;
	//std::vector<float> tex;
	void VertexData(const std::string& arrayName)
	{
		std::fstream file;
		file.open("src/VertexData.txt", std::ios::in);
		if (file.fail())
		{
			std::cout << "Vertex data file didn't open" << std::endl;
			file.clear();
		}
		std::string line;
		bool Flag[4] = { false,false,false,false }; //objData,pos,norm,tex
		//num_wrds = 0;
		while (std::getline(file, line))
		{
			if (line.find(("~"+arrayName).c_str()) != std::string::npos)
			{
				Flag[0] = true;
			}
			else if (Flag[0] && line.find("#pos") != std::string::npos)
			{
				Flag[1] = true;
			}
			else if (Flag[0] && line.find("#norm") != std::string::npos)
			{
				Flag[1] = false;
				Flag[2] = true;
			}
			else if (Flag[0] && line.find("#tex") != std::string::npos)
			{
				Flag[1] = false;
				Flag[2] = false;
				Flag[3] = true;
			}
			else if (Flag[0] && line.find("~") != std::string::npos)
			{
				break;
			}
			else if (Flag[0] && Flag[1])
			{
				std::string wrd;
				std::stringstream ss(line);
				while (ss >> wrd)
				{
					data[0].push_back(atof(wrd.c_str()));
					dObj.num_wrds++;
				}
			}
			else if (Flag[0] && Flag[2])
			{
				std::string wrd;
				std::stringstream ss(line);
				while (ss >> wrd)
					data[1].push_back(atof(wrd.c_str()));
			}
			else if (Flag[0] && Flag[3])
			{
				std::string wrd;
				std::stringstream ss(line);
				while (ss >> wrd)
					data[2].push_back(atof(wrd.c_str()));
			}
		}
		for (unsigned int i = 0; i < 3; ++i)
		{
			if (data[i].empty())
				priority[i] = false;
			else
				priority[i] = true;
		}
		for (unsigned int i = 0, j=0; i < 3; ++i)
		{
			if (priority[i])
			{
				dObj.vData[j] = data[i].data();
				dObj.dSize[j++] = data[i].size()*sizeof(float);
			}
		}
	}
public:
	VertexBuffer(const void* data,size_t size)
	{
		GLCall(glGenBuffers(1, &m_RendererID));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	}
	VertexBuffer(const std::string& arrayName)
	{
		VertexData(arrayName);
		unsigned int num = int(priority[0])+int(priority[1])+int(priority[2]);
		GLCall(glGenBuffers(1, &m_RendererID));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		GLCall(glBufferData(GL_ARRAY_BUFFER, dObj.dSize[0] + dObj.dSize[1] + dObj.dSize[2], NULL, GL_STATIC_DRAW));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0,dObj.dSize[0], dObj.vData[0]));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, dObj.dSize[0],dObj.dSize[1], dObj.vData[1]));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, dObj.dSize[0]+ dObj.dSize[1], dObj.dSize[2], dObj.vData[2]));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
	~VertexBuffer()
	{
		GLCall(glDeleteBuffers(1, &m_RendererID));
	}
	unsigned int GetWrdNum() const
	{
		return dObj.num_wrds;
	}
	unsigned int GetID()
	{
		return m_RendererID;
	}
	void Bind() const
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	}
	void Unbind() const 
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
};
