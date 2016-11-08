#include "ScreenQuad.h"
#include "Renderer.h"

using namespace oglplus;
/***********************************************************************************/
ScreenQuad::ScreenQuad() {
	m_vao.Bind();
	
	m_vbo.Bind(BufferTarget::Array);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_quadVertices), &m_quadVertices, GL_STATIC_DRAW);

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	// Tex-Coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
}

/***********************************************************************************/
ScreenQuad::~ScreenQuad() {
}

/***********************************************************************************/
void ScreenQuad::Draw() const {
	m_vao.Bind();
	//glBindTexture(GL_TEXTURE_2D, GetGLName(fb));
	Renderer::DrawArrays(PrimitiveType::Triangles, 0, 6);
}
