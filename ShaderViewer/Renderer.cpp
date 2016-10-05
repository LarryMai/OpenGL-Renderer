#include "Renderer.h"
#include <oglplus/all.hpp>

using namespace oglplus;
Renderer::Renderer(const GLuint width, const GLuint height, const RenderMode mode) : m_lastFrame(0.0f), m_deltaTime(0.0f) {

	GLAPIInitializer api_init;

	m_glContext.Viewport(width, height);

	// OpenGL options (not state-dependent)
	m_glContext.Enable(Capability::Multisample);

	m_glContext.Enable(Capability::CullFace);
	m_glContext.CullFace(Face::Back); // Back-face culling

	m_glContext.Enable(Capability::Blend);
	m_glContext.BlendFunc(0, BlendFunction::SrcAlpha, BlendFunction::OneMinusSrcAlpha); // Alpha blending
	
	m_glContext.Enable(Capability::FramebufferSRGB); // Gamma Correction

	m_glContext.DepthFunc(CompareFunction::Less);

	switch (mode) {
	case FILLED:
		m_glContext.PolygonMode(Face::FrontAndBack, PolygonMode::Fill);
		break;
	case WIREFRAME:
		m_glContext.PolygonMode(Face::FrontAndBack, PolygonMode::Line);
		break;
	case POINTS:
		m_glContext.PolygonMode(Face::FrontAndBack, PolygonMode::Point);
		break;
	}

	m_timer.Init();

	std::cout << "\nOpenGL successfully initialized!" << std::endl;
}

Renderer::~Renderer() {
}

void Renderer::PreRender(const double currentTime) {
	// Calculate deltatime of current frame
	GLfloat currentFrame = currentTime;
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	// Update timer
	m_timer.Update();
}
