#pragma once
//#include "IRenderer.h"

#include <oglplus/gl.hpp>
#include <oglplus/context.hpp>

class Renderer {
public:
	const enum RenderMode {
		FILLED = oglplus::PolygonMode::Fill,
		WIREFRAME = oglplus::PolygonMode::Line,
		POINTS = oglplus::PolygonMode::Point
	};

	Renderer(const GLuint width, const GLuint height, const RenderMode mode);
	~Renderer();

	Renderer(const Renderer& other) = delete;
	void operator=(const Renderer& rhs) = delete;

	void PreRender(const double currentTime);

private:
	oglplus::Context m_glContext;

	GLfloat m_deltaTime, m_lastFrame;
};

