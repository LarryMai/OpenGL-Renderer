#pragma once
#include "IRenderer.h"

#include <oglplus/gl.hpp>
#include <oglplus/context.hpp>

class Renderer : public IRenderer {
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

	void PreRender(const double currentTime) override;

	float GetFPS() const { return m_timer.GetFPS(); }
	GLfloat GetDeltaTime() const { return m_deltaTime; }

private:
	oglplus::Context m_glContext;

	GLfloat m_deltaTime, m_lastFrame;
	GLuint m_width, m_height;
};

