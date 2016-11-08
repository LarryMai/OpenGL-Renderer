#include "Renderer.h"

#include <iostream>
#include <array>

using namespace oglplus;

/***********************************************************************************/
Renderer::Renderer(const GLuint width, const GLuint height) : m_lastFrame(0.0f), m_deltaTime(0.0f) {

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

	m_glContext.PolygonMode(PolygonMode::Fill);

	/*
	// The gbuffer and depthbuffer obviously have to be initialized after OpenGL is initialized...
	// In order to avoid this crap with pointers, std::optional can apparently be used but msvc doesn't have it yet -_-
	m_gBuffer = std::make_unique<Framebuffer>();
	m_depthBuffer = std::make_shared<Renderbuffer>();

	// Init deferred renderer stuff
	m_gBuffer->Bind(Framebuffer::Target::Draw);
	
	m_gPosition.Bind(Texture::Target::_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer->AttachTexture2D(Framebuffer::Target::Draw, FramebufferAttachment::Color, Texture::Target::_2D, m_gPosition, 0);

	m_gNormal.Bind(Texture::Target::_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer->AttachTexture2D(Framebuffer::Target::Draw, FramebufferAttachment::Color1, Texture::Target::_2D, m_gNormal, 0);

	m_gColorSpecular.Bind(Texture::Target::_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer->AttachTexture2D(Framebuffer::Target::Draw, FramebufferAttachment::Color2, Texture::Target::_2D, m_gColorSpecular, 0);

	// Tell OpenGL what color attachments we are using
	std::array<GLuint, 3> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(attachments.size(), attachments.data());

	// Attach depth buffer (Renderbuffer)
	m_depthBuffer->Bind(RenderbufferTarget::Renderbuffer);
	m_depthBuffer->Storage(RenderbufferTarget::Renderbuffer, PixelDataInternalFormat::DepthComponent, width, height);
	m_gBuffer->AttachRenderbuffer(Framebuffer::Target::Draw, FramebufferAttachment::Depth, *m_depthBuffer); // Get the underlying data from shared_ptr

	// Make sure framebuffer is complete
	if(!m_gBuffer->IsComplete(FramebufferTarget::Draw)) {
		std::cerr << "m_gBuffer not complete!\n";
		throw std::runtime_error("m_gbuffer not complete!");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
*	*/
	m_timer.Init();

	std::cout << "\nOpenGL successfully initialized: \n" << m_glContext.Version() << "\nGPU Vendor: " << m_glContext.Vendor() << std::endl;
}

/***********************************************************************************/
Renderer::~Renderer() {
}

/***********************************************************************************/
void Renderer::PreRender(const double currentTime) {
	// Calculate deltatime of current frame
	GLfloat currentFrame = currentTime;
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	// Update timer
	m_timer.Update();
}

/***********************************************************************************/
void Renderer::BeginGeometryPass() const {
	
	glBindFramebuffer(GL_FRAMEBUFFER, GetGLName(*m_gBuffer));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Enable depth testing for 3D stuff
	EnableCapability(Capability::DepthTest);
}

/***********************************************************************************/
void Renderer::BeginLightingPass() const {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	oglplus::Texture::Active(0);
	m_gPosition.Bind(TextureTarget::_2D);
	
	oglplus::Texture::Active(1);
	m_gNormal.Bind(TextureTarget::_2D);

	oglplus::Texture::Active(2);
	m_gColorSpecular.Bind(TextureTarget::_2D);
}
