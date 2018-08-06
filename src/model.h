#ifndef MODEL_H
#define MODEL_H

#include"Renderer.h"
#include"Shader_s.h"
#include"mesh.h"
#include"stb_image.h"
#include"assimp\Importer.hpp"
#include"assimp\scene.h"
#include"assimp\postprocess.h"
#include<map>
#include<vector>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
public:
	std::vector<Texture_m> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;

	Model(const std::string &path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			meshes[i].Draw(shader);
		}
	}

private:
	void loadModel(const std::string &path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString()<<std::endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}
	void processNode(aiNode *node, const aiScene *scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			processNode(node->mChildren[i], scene);
		}
	}
	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		std::vector<Vertex_m> vertices_m;
		std::vector<unsigned int> indices_m;
		std::vector<Texture_m> textures_m;

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertex_m vertex;
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;

			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;

			vertices_m.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
				indices_m.push_back(face.mIndices[j]);
		}

		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture_m> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures_m.insert(textures_m.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture_m> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular");
		textures_m.insert(textures_m.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture_m> reflectMaps = loadMaterialTexture(material, aiTextureType_AMBIENT, "texture_reflact");
		textures_m.insert(textures_m.end(), reflectMaps.begin(), reflectMaps.end());

		return Mesh(vertices_m, indices_m, textures_m);
	}

	std::vector<Texture_m> loadMaterialTexture(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture_m> texture;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			std::cout << "----  " << str.C_Str() << "  ---" << std::endl;
			
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); ++j)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					texture.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				Texture_m texta;
				texta.id = TextureFromFile(str.C_Str(), this->directory);
				texta.type = typeName;
				texta.path = str.C_Str();
				std::cout << texta.id << " " << texta.type << " " << texta.path << std::endl;
				texture.push_back(texta);
				textures_loaded.push_back(texta);
			}
		}
		return texture;
	}
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gama)
{
	std::string filename = std::string(path);
	filename = directory + "/" + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		std::cout << "data loaded" << std::endl;
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		std::cout << "Falied to load at path." << std::endl;
	}
	stbi_image_free(data);
	return textureID;
}

#endif