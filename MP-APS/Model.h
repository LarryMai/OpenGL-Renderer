#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"

#include <string_view>
#include <unordered_set>

class Model {
public:
	Model(const std::string_view Path, const std::string_view Name, const bool flipWindingOrder = false);
	Model(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures) noexcept;
	Model(const Mesh& mesh);
	~Model() = default;

	void SetInstancing(const std::initializer_list<glm::vec3>& instanceOffsets);

	void Draw(GLShaderProgram* shader);
	void DrawInstanced(GLShaderProgram* shader);

	// Transformations
	void Scale(const glm::vec3& scale) noexcept { m_scale = scale; }
	void Rotate(const float radians, const glm::vec3& axis) noexcept { m_radians = radians; m_axis = axis; }
	void Translate(const glm::vec3& pos) noexcept { m_position = pos; }
	// Build model matrix from stored transformation data
	glm::mat4 GetModelMatrix() const noexcept;

private:
	bool loadModel(const std::string_view Path, const bool flipWindingOrder);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<GLTexture> loadMatTextures(aiMaterial* mat, aiTextureType type, const std::string_view samplerName);

	glm::vec3 m_scale, m_position, m_axis;
	float m_radians;

	std::vector<Mesh> m_meshes;
	std::vector<GLTexture> m_loadedTextures;

	const std::string m_name;
	std::string m_path;
};

