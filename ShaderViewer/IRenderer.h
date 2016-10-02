#pragma once
#include "Timer.h"

// Abstract class for child renderers (OpenGL or DirectX)
class IRenderer {
public:
	IRenderer();
	virtual ~IRenderer();

	IRenderer(const IRenderer& other) = delete;
	void operator=(const IRenderer& rhs) = delete;

protected:
	// Updates timers and calculates d(t)
	virtual void PreRender(const double currentTime) = 0;

	Timer m_timer;
};

