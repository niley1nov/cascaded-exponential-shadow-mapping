#ifndef MESH_H
#define MESH_H

#include"Renderer.h"
#include"Shader_s.h"
#include"VertexArray.h"
#include"VertexBuffer.h"
#include"IndexBuffer.h"

struct Vertex_m
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 bitangent;
};

struct Texture_m
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	std::vector<Vertex_m> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Texture_m> m_textures;
	unsigned int VAO;

	Mesh(std::vector<Vertex_m> vertices, std::vector<unsigned int> indices, std::vector<Texture_m> textures)
	{
		m_vertices = vertices;
		m_indices = indices;
		m_textures = textures;
		setUpMesh();
	}

	void Draw(Shader shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		//unsigned int normalNr = 1;
		//unsigned int heightNr = 1;
		unsigned int reflactNr = 1;
		for (unsigned int i = 0; i < m_textures.size(); ++i)
		{
			//GLCall(glActiveTexture(GL_TEXTURE0 + i));
			unsigned int number;
			std::string name = m_textures[i].type;
			if (name == "texture_diffuse")
				number = 1;
			else if (name == "texture_specular")
				number = 2;
			else if (name == "texture_reflact")
				number = 3;
			//else if (name == "texture_normal")
			//	number = std::to_string(normalNr++);
			//else if (name == "texture_height")
			//	number = std::to_string(heightNr++);
			
			//std::cout << "i = " << i << std::endl;
			//std::cout << "texture name - " << name + number << std::endl;
			GLCall(glActiveTexture(GL_TEXTURE0+number));
			GLCall(glBindTexture(GL_TEXTURE_2D, m_textures[i].id));
			shader.setInt(name, number);
		}

		GLCall(glBindVertexArray(VAO));
		GLCall(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0));
		
		GLCall(glBindVertexArray(0));
		GLCall(glActiveTexture(GL_TEXTURE0));
	}
	void AddInstancedMat4(unsigned int BuffID, unsigned int index)
	{
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, BuffID);

		GLCall(glEnableVertexAttribArray(index));
		GLCall(glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0));
		GLCall(glEnableVertexAttribArray(1 + index));
		GLCall(glVertexAttribPointer(index+1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))));
		GLCall(glEnableVertexAttribArray(2 + index));
		GLCall(glVertexAttribPointer(index+2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))));
		GLCall(glEnableVertexAttribArray(3 + index));
		GLCall(glVertexAttribPointer(index+3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))));

		GLCall(glVertexAttribDivisor(index, 1));
		GLCall(glVertexAttribDivisor(index + 1, 1));
		GLCall(glVertexAttribDivisor(index + 2, 1));
		GLCall(glVertexAttribDivisor(index + 3, 1));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

private:
	unsigned int VBO, EBO;
	void setUpMesh()
	{
		GLCall(glGenVertexArrays(1, &VAO));
		GLCall(glGenBuffers(1, &VBO));
		GLCall(glGenBuffers(1, &EBO));

		GLCall(glBindVertexArray(VAO));

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		GLCall(glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex_m), &m_vertices[0], GL_STATIC_DRAW));

		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW));
		
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_m), (void*)offsetof(Vertex_m,Position)));

		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_m), (void*)offsetof(Vertex_m, Normal)));

		GLCall(glEnableVertexAttribArray(2));
		GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_m), (void*)offsetof(Vertex_m, TexCoords)));

		GLCall(glEnableVertexAttribArray(3));
		GLCall(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_m), (void*)offsetof(Vertex_m, Tangent)));

		GLCall(glEnableVertexAttribArray(4));
		GLCall(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_m), (void*)offsetof(Vertex_m, bitangent)));

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		GLCall(glBindVertexArray(0));
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};

#endif