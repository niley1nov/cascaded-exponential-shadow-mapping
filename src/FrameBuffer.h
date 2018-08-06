#pragma once

#include"Renderer.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID;
public:
	unsigned int TexColor, RenderBuffer;
	FrameBuffer(const unsigned int& width,const unsigned int& height,unsigned int attachNum)
	{
		GLCall(glGenFramebuffers(1, &m_RendererID));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

		GLCall(glGenTextures(1, &TexColor));
		GLCall(glBindTexture(GL_TEXTURE_2D, TexColor));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachNum, GL_TEXTURE_2D, TexColor, 0));

		GLCall(glGenRenderbuffers(1, &RenderBuffer));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer));
		GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));

		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBuffer));

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMBUFFER - Framebuffer is not complete." << std::endl;
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	}
	FrameBuffer(const unsigned int& width, const unsigned int& height, const unsigned int& samples, const int attachNum)
	{
		GLCall(glGenFramebuffers(1, &m_RendererID));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
		
		GLCall(glGenTextures(1, &TexColor));
		GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, TexColor));
		GLCall(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE));
		
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachNum, GL_TEXTURE_2D_MULTISAMPLE, TexColor, NULL));
		
		GLCall(glGenRenderbuffers(1, &RenderBuffer));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer));
		GLCall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height));
		
		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBuffer));
		
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMBUFFER - Framebuffer is not complete." << std::endl;
		GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	~FrameBuffer()
	{
		GLCall(glDeleteFramebuffers(1, &m_RendererID));
	}
	unsigned int GetID()
	{
		return m_RendererID;
	}
	void Bind()
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
	}
	void Unbind()
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	void BindTex(unsigned int num = 0)
	{
		glActiveTexture(GL_TEXTURE0 + num);
		glBindTexture(GL_TEXTURE_2D, TexColor);
	}
	void UnbindTex()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TexColor);
	}
};

class CascadeFBO
{
private:
	unsigned int m_RendererID;
	unsigned int m_ShadowMap[3];
	unsigned int m_Count;
	float *cascadeEnd;
	float ar;
	glm::vec4 modeldFrusCorns[3][8];

public:
	CascadeFBO(unsigned int width, unsigned int height, unsigned int count, float *CasEnds,float arr)
		:m_Count(count), cascadeEnd(CasEnds), ar(arr)
	{
		GLCall(glGenFramebuffers(1, &m_RendererID));
		GLCall(glGenTextures(count, m_ShadowMap));

		for (unsigned int i = 0; i < count; ++i)
		{
			GLCall(glBindTexture(GL_TEXTURE_2D, m_ShadowMap[i]));
			GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
		}
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ShadowMap[0], 0));

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("FB error, status: 0x%x\n", Status);
		}
	}
	~CascadeFBO()
	{
		GLCall(glDeleteFramebuffers(1, &m_RendererID));
	}
	void CalcOrtProjs(const glm::mat4& Cam, const glm::mat4 *vView, const float& angle)
	{
		glm::mat4 CamInv = glm::inverse(Cam);

		float tanHalfHFOV = glm::tan(glm::radians(angle / 2.0f))/ar;
		float tanHalfVFOV = glm::tan(glm::radians(angle*ar / 2.0f))/ar;

		for (unsigned int i = 0; i < m_Count; ++i)
		{
			float xn = cascadeEnd[i] * tanHalfHFOV;
			float xf = cascadeEnd[i + 1] * tanHalfHFOV;
			float yn = cascadeEnd[i] * tanHalfVFOV;
			float yf = cascadeEnd[i + 1] * tanHalfVFOV;

			glm::vec4 frustrumCorners[8] = {
				//near face
				glm::vec4(xn,yn,-cascadeEnd[i],1.0f),
				glm::vec4(-xn,yn,-cascadeEnd[i],1.0f),
				glm::vec4(xn,-yn,-cascadeEnd[i],1.0f),
				glm::vec4(-xn,-yn,-cascadeEnd[i],1.0f),
				//far Face
				glm::vec4(xf,yf,-cascadeEnd[i + 1],1.0f),
				glm::vec4(-xf,yf,-cascadeEnd[i + 1],1.0f),
				glm::vec4(xf,-yf,-cascadeEnd[i + 1],1.0f),
				glm::vec4(-xf,-yf,-cascadeEnd[i + 1],1.0f)
			};

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();

			for (unsigned int j = 0; j < 8; ++j)
			{
				glm::vec4 vW = CamInv * frustrumCorners[j];
				frustrumCorners[j] = vView[0] * vW;

				minX = glm::min(minX, frustrumCorners[j].x);
				maxX = glm::max(maxX, frustrumCorners[j].x);
				minY = glm::min(minY, frustrumCorners[j].y);
				maxY = glm::max(maxY, frustrumCorners[j].y);
				minZ = glm::min(minZ, frustrumCorners[j].z);
				maxZ = glm::max(maxZ, frustrumCorners[j].z);

				if (j == 7)
				{
					modeldFrusCorns[i][0] = glm::inverse(vView[0])*glm::vec4(maxX, maxY, minZ, 1.0f);
					modeldFrusCorns[i][1] = glm::inverse(vView[0])*glm::vec4(minX, maxY, minZ, 1.0f);
					modeldFrusCorns[i][2] = glm::inverse(vView[0])*glm::vec4(maxX, minY, minZ, 1.0f);
					modeldFrusCorns[i][3] = glm::inverse(vView[0])*glm::vec4(minX, minY, minZ, 1.0f);
					modeldFrusCorns[i][4] = glm::inverse(vView[0])*glm::vec4(maxX, maxY, maxZ, 1.0f);
					modeldFrusCorns[i][5] = glm::inverse(vView[0])*glm::vec4(minX, maxY, maxZ, 1.0f);
					modeldFrusCorns[i][6] = glm::inverse(vView[0])*glm::vec4(maxX, minY, maxZ, 1.0f);
					modeldFrusCorns[i][7] = glm::inverse(vView[0])*glm::vec4(minX, minY, maxZ, 1.0f);
				}
			}
		}
	}
	glm::mat4 GetProjMat(glm::mat4& view,unsigned int index)
	{
		return glm::ortho((view*modeldFrusCorns[index][1]).x, (view*modeldFrusCorns[index][0]).x, (view*modeldFrusCorns[index][2]).y, (view*modeldFrusCorns[index][0]).y, (view*modeldFrusCorns[index][0]).z, (view*modeldFrusCorns[index][4]).z);
	}
	float GetRatio(glm::mat4& view,int index)
	{
		if (index > -1 && index < 4)
		{
			return((-(view*modeldFrusCorns[index][0]).z + (view*modeldFrusCorns[index][4]).z) );
		}
		else
		{
			return 1.0f;
		}
	}
	glm::vec3 GetModlCent(unsigned int index)
	{
		glm::vec4 temp = glm::vec4(0.0f);
		for (unsigned int i = 0; i < 8; ++i)
		{
			temp += modeldFrusCorns[index][i];
		}
		glm::vec4 temp2 = temp / 8.0f;
		return glm::vec3(temp2);
	}
	void BindForWriting(unsigned int index)
	{
		ASSERT(index < m_Count)
			
		//GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID));
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ShadowMap[index], 0));
	}
	void BindForReading(unsigned int index)
	{
		for (unsigned int i = 0; i < m_Count; ++i)
		{
			GLCall(glActiveTexture(GL_TEXTURE0+i+index));
			GLCall(glBindTexture(GL_TEXTURE_2D, m_ShadowMap[i]));
		}
	}
	unsigned int GetID()
	{
		return m_RendererID;
	}
	void Bind()
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
	}
	void Unbind()
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
};