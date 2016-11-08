#pragma once
#include <array>

#include <oglplus/gl.hpp>
#include <oglplus/vertex_array.hpp>
#include <oglplus/buffer.hpp>

// Used to render quads in NDC. Can be used for post-processing or deferred rendering.
class ScreenQuad {
public:
	ScreenQuad();
	~ScreenQuad();

	void Draw() const;

private:
	oglplus::VertexArray m_vao;
	oglplus::Buffer m_vbo;

	// Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates
	const std::array<GLfloat, 24> m_quadVertices = {
		// Positions   TexCoords
		// Triangle 1
		-1.0f,  1.0f,  0.0f, 1.0f, // Top-left
		-1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
		 1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right

		// Triangle 2
		-1.0f,  1.0f,  0.0f, 1.0f, // Top-left
		 1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
		 1.0f,  1.0f,  1.0f, 1.0f  // Top-right
	};
};

