#include "Mesh.h"
#include "Renderer.h"


/***********************************************************************************/
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures) :
	m_vertices(vertices), 
	m_indices(indices), 
	m_textures(textures) {

	//Construct mesh
	setupMesh();
}

/***********************************************************************************/
Mesh::~Mesh() {
}

/***********************************************************************************/
void Mesh::SetInstancing(const std::initializer_list<glm::vec3>& args) {
	
	// Iterate over positions passed in
	for (const auto& it : args) {
		m_instanceOffsets.push_back(it);
	}

	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_instanceOffsets.size(), &m_instanceOffsets.at(0), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Vertex Instance offset
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(3, 1); // Tell OpenGL this is an instanced vertex attribute.

	glBindVertexArray(0);
}

/***********************************************************************************/
void Mesh::Draw(const Shader& shader) {
	
	bindTextures(shader);

	// Draw mesh
	glBindVertexArray(m_vao);
	Renderer::DrawElements(oglplus::PrimitiveType::Triangles, m_indices.size(), oglplus::DataType::UnsignedInt);
	glBindVertexArray(0);
}

/***********************************************************************************/
void Mesh::DrawInstanced(const Shader& shader) {
	
	bindTextures(shader);

	// Draw instanced mesh
	glBindVertexArray(m_vao);
	Renderer::DrawElementsInstanced(oglplus::PrimitiveType::Triangles, m_indices.size(), oglplus::DataType::UnsignedInt, m_instanceOffsets.size());
	glBindVertexArray(0);
}

/***********************************************************************************/
void Mesh::bindTextures(const Shader &shader) {
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint reflectanceNr = 1;
	GLuint normalNr = 1;

	GLuint index = 0;
	for (auto& it : m_textures) {
		oglplus::Texture::Active(0 + index); // Activate proper texture unit before binding
											  // Retrieve texture number (the N in diffuse_textureN)
		std::string name = it.GetSampler();
		std::string number;
		// ALWAYS USE POST-INCREMENT HERE
		if (name == "texture_diffuse") {
			number = std::to_string(diffuseNr++);
		}
		if (name == "texture_specular") {
			number = std::to_string(specularNr++);
		}
		if (name == "texture_reflectance") {
			number = std::to_string(reflectanceNr++);
		}
		if (name == "texture_normal") {
			number = std::to_string(normalNr++);
		}

		glUniform1i(shader.GetUniformLoc(name + number), index);
		glBindTexture(GL_TEXTURE_2D, it.GetTexture());

		++index;
	}

	oglplus::Texture::Active(0);
}

/***********************************************************************************/
void Mesh::setupMesh() {

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices.at(0), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices.at(0), GL_STATIC_DRAW);

	// Vertex Attributes

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<GLvoid*>(0));
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, Normal)));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}
