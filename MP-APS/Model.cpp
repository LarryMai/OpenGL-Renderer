#include "Model.h"
#include "GL/GLRenderer.h"

#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

/***********************************************************************************/
Model::Model(const std::string_view Path, const std::string_view Name, const bool flipWindingOrder) : m_name(Name), m_path(Path) {
	
	if(!loadModel(Path, flipWindingOrder)) {
		std::cerr << "Failed to load: " << Name << '\n';
	}


}

/***********************************************************************************/
Model::Model(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures) noexcept {
	m_meshes.emplace_back(vertices, indices, textures);
}

/***********************************************************************************/
Model::Model(const Mesh& mesh) {
	m_meshes.push_back(std::move(mesh));
}

/***********************************************************************************/
void Model::SetInstancing(const std::initializer_list<glm::vec3>& instanceOffsets) {
	// Pass list to each mesh
	for (auto& mesh : m_meshes) {
		mesh.SetInstancing(instanceOffsets);
	}
}

/***********************************************************************************/
void Model::Draw(GLShaderProgram* shader) {
	for (auto& mesh : m_meshes) {
		mesh.Draw(shader);
	}
}

/***********************************************************************************/
void Model::DrawInstanced(GLShaderProgram* shader) {
	for (auto& mesh : m_meshes) {
		mesh.DrawInstanced(shader);
	}
}

/***********************************************************************************/
glm::mat4 Model::GetModelMatrix() const noexcept {
	glm::mat4 modelMatrix;
		
	modelMatrix = glm::scale(modelMatrix, m_scale);

	return modelMatrix;
}

/***********************************************************************************/
bool Model::loadModel(const std::string_view Path, const bool flipWindingOrder) {
	std::cout << "---Loading model: " << m_name << '\n';
	
	Assimp::Importer importer;
	const aiScene* scene = nullptr;

	if (flipWindingOrder) {
		scene = importer.ReadFile(Path.data(), aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenUVCoords |
			aiProcess_SortByPType |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_FindInvalidData |
			aiProcess_FlipUVs |
			aiProcess_FlipWindingOrder | // Reverse back-face culling
			aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes |
			aiProcess_SplitLargeMeshes);
	} 
	else {
		scene = importer.ReadFile(Path.data(), aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenUVCoords |
			aiProcess_SortByPType |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_FindInvalidData |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_ImproveCacheLocality |
			aiProcess_OptimizeMeshes |
			aiProcess_SplitLargeMeshes);
	}

	// Check if scene is not null and model is done loading
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Assimp Error for " << m_name << ": " << importer.GetErrorString() << '\n';
		importer.FreeScene();
		return false;
	}

	m_path = Path.substr(0, Path.find_last_of('/'));
	processNode(scene->mRootNode, scene);

	importer.FreeScene();
	std::cout << "---Loaded Model: " << m_name << '\n';
	return true;
}

/***********************************************************************************/
void Model::processNode(aiNode* node, const aiScene* scene) {

	// Process all node meshes
	for (std::size_t i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh, scene));
	}

	// Process their children
	for (std::size_t i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

/***********************************************************************************/
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<GLTexture> textures;

	for (std::size_t i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;

		if (mesh->HasPositions()) {
			vertex.Position->x = mesh->mVertices[i].x;
			vertex.Position->y = mesh->mVertices[i].y;
			vertex.Position->z = mesh->mVertices[i].z;

			// Construct bounding box
		}
		
		if (mesh->HasNormals()) {
			vertex.Normal->x = mesh->mNormals[i].x;
			vertex.Normal->y = mesh->mNormals[i].y;
			vertex.Normal->z = mesh->mNormals[i].z;
		}
		
		if (mesh->HasTangentsAndBitangents()) {
			vertex.Tangent->x = mesh->mTangents[i].x;
			vertex.Tangent->y = mesh->mTangents[i].y;
			vertex.Tangent->z = mesh->mTangents[i].z;

			vertex.Bitangent->x = mesh->mBitangents[i].x;
			vertex.Bitangent->y = mesh->mBitangents[i].y;
			vertex.Bitangent->z = mesh->mBitangents[i].z;
		}

		if (mesh->HasTextureCoords(0)) {
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vertex.TexCoords->x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords->y = mesh->mTextureCoords[0][i].y;
		} else {
			vertex.TexCoords = glm::vec2(0.0f);
		}
		vertices.push_back(vertex);
	}

	// Get indices from each face
	for (std::size_t i = 0; i < mesh->mNumFaces; ++i) {
		const aiFace face = mesh->mFaces[i];
		
		for (std::size_t j = 0; j < face.mNumIndices; ++j) {
			indices.emplace_back(face.mIndices[j]);
		}
	}

	// Process materials
	if (mesh->mMaterialIndex >= 0) {
		auto* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		const auto diffuseMaps = loadMatTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		
		// 2. Specular maps
		const auto specularMaps = loadMatTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	// Return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

/***********************************************************************************/
std::vector<GLTexture> Model::loadMatTextures(aiMaterial* mat, aiTextureType type, const std::string_view samplerName) {
	
	std::vector<GLTexture> textures;
	
	// Get all textures
	for (std::size_t c = 0; c < mat->GetTextureCount(type); ++c) {
		aiString texturePath;
		mat->GetTexture(type, c, &texturePath);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (auto& loadedTex : m_loadedTextures) {

			if (std::strcmp(loadedTex.GetRelativePath().c_str(), texturePath.C_Str()) == 0) {
				textures.push_back(loadedTex);
				skip = true;
				break;
			}
		}

		if (!skip) {   // If texture hasn't been loaded already, load it
			
			const auto texDirPrefix = m_path + "/"; // Get directory path and append forward-slash
			GLTexture texture(texDirPrefix, texturePath.C_Str(), samplerName, GLTexture::WrapMode::REPEAT);
			
			std::cout << "GLTexture not found! Adding: " << texDirPrefix + texturePath.C_Str() << '\n';

			textures.push_back(texture);
			m_loadedTextures.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}