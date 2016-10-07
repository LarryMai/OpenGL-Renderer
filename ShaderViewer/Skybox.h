#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <oglplus\vertex_array.hpp>

#include <array>

#include "Shader.h"

class Skybox {
public:
	Skybox(const std::string& TextureDirectory);
	Skybox(const Skybox&) = delete; // No copy c-tor
	~Skybox();

	// Bind texture if passing to other shaders
	void BindTexture() { glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID); }
	void Draw(const glm::mat4& CameraMatrix, const glm::mat4& ProjectionMat);

private:
	std::array<std::string, 6> m_faces; // A skybox only ever has 6 faces, so an array is fine (and safer perhaps)
	
	GLuint m_textureID;
	oglplus::VertexArray m_vao;
	oglplus::Buffer m_vbo, m_indices;
	Shader m_shader;

	const GLfloat m_skyboxVertices[108] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	const GLuint m_skyboxIndices[6 * 5] = {
		1, 3, 5, 7, 9,
		4, 6, 0, 2, 9,
		2, 6, 3, 7, 9,
		4, 0, 5, 1, 9,
		5, 7, 4, 6, 9,
		0, 2, 1, 3, 9
	};
};

