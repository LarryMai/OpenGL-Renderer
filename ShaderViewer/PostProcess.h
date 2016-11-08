#pragma once

#include <string>
#include <gl/glew.h>

#include "Shader.h"
#include "FrameBuffer.h"

class PostProcess {
public:
	PostProcess(const std::string& screenVertShader, const std::string& screenPixelShader);
	~PostProcess();

	// Render processed image to a screen quad
	void RendertoScreen(const FrameBuffer& fb) const;

private:
	Shader m_screenShader;
	GLuint m_screenVAO, m_screenVBO;
};

