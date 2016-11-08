#pragma once
#include "IRenderer.h"

#include <oglplus/gl.hpp>
#include <oglplus/context.hpp>
#include <oglplus/framebuffer.hpp>
#include <oglplus/renderbuffer.hpp>
#include <oglplus/texture.hpp>

#include <memory>

/***********************************************************************************/
class Renderer : public IRenderer {
public:
	Renderer(const GLuint width, const GLuint height);
	~Renderer();

	Renderer(const Renderer& other) = delete;
	void operator=(const Renderer& rhs) = delete;

	static void EnableCapability(const oglplus::Capability& c) { m_glContext.Enable(c); }
	static void DisableCapability(const oglplus::Capability& c) { m_glContext.Disable(c); }
	static void SetDepthFunc(const oglplus::CompareFunction& c) { m_glContext.DepthFunc(c); }
	static void SetRenderMode(const oglplus::PolygonMode& mode) { m_glContext.PolygonMode(mode); }

	static void DrawElements(const oglplus::PrimitiveType& type, const size_t numElements, const oglplus::DataType& dataType) { m_glContext.DrawElements(type, numElements, dataType); }
	static void DrawElementsInstanced(const oglplus::PrimitiveType& type, const size_t numElements, const oglplus::DataType& dataType, const size_t instanceCount) { m_glContext.DrawElementsInstanced(type, numElements, dataType, instanceCount); }
	static void DrawArrays(const oglplus::PrimitiveType& type, const size_t first, const size_t count) { m_glContext.DrawArrays(type, first, count); }
	
	static void ClearColor(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a) { m_glContext.ClearColor(r, g, b, a); }

	// Update timers
	void PreRender(const double currentTime) override;

	void BeginGeometryPass() const;
	// Bind and activate G-Buffer textures
	void BeginLightingPass() const;

	float GetFPS() const { return m_timer.GetFPS(); }
	GLfloat GetDeltaTime() const { return m_deltaTime; }

private:
	static oglplus::Context m_glContext;

	// Stuff for deferred shading.
	// http://www.learnopengl.com/#!Advanced-Lighting/Deferred-Shading
	std::unique_ptr<oglplus::Framebuffer> m_gBuffer;
	std::shared_ptr<oglplus::Renderbuffer> m_depthBuffer;
	oglplus::Texture m_gPosition, m_gNormal, m_gColorSpecular;

	GLfloat m_deltaTime, m_lastFrame;
};

