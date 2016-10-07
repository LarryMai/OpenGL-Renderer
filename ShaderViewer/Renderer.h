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

	static void EnableCapability(const oglplus::Capability& c) { m_glContext.Enable(c); }
	static void DisableCapability(const oglplus::Capability& c) { m_glContext.Disable(c); }
	static void SetDepthFunc(const oglplus::CompareFunction& c) { m_glContext.DepthFunc(c); }
	static void DrawElements(const oglplus::PrimitiveType& type, const size_t numElements, const oglplus::DataType& dataType) { m_glContext.DrawElements(type, numElements, dataType); }
	static void DrawArrays(const oglplus::PrimitiveType& type, const size_t first, const size_t count) { m_glContext.DrawArrays(type, first, count); }
	static void ClearColor(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a) { m_glContext.ClearColor(r, g, b, a); }

	void PreRender(const double currentTime) override;

	float GetFPS() const { return m_timer.GetFPS(); }
	GLfloat GetDeltaTime() const { return m_deltaTime; }

private:
	static oglplus::Context m_glContext;

	GLfloat m_deltaTime, m_lastFrame;
	GLuint m_width, m_height;
};

