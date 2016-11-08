#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>

#include "Mesh.h"

class Model {
public:
	Model(const std::string& Path);
	~Model();

	void SetInstancing(const std::initializer_list<glm::vec3>& instanceOffsets);

	void Draw(const Shader& shader);
	void DrawInstanced(const Shader& shader);

private:
	void loadModel(const std::string& Path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMatTextures(aiMaterial* mat, aiTextureType type, const std::string& samplerName);

	std::vector<Mesh> m_meshes;
	std::vector<Texture> m_loadedTextures;
	
	// Array of pointers to all loaded models (easy way to iterate through all models)
	// Need to add shared_from_this()
	static std::vector<Model*> m_modelArray;

	std::string m_directory;
	bool m_instanced;
};