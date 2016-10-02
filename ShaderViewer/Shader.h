#pragma once
#include <GL\glew.h>
#include <oglplus/all.hpp>
#include <oglplus/shader.hpp>

class Shader {
public:
	Shader(const std::string& commonData, const std::string& VertexShader, const std::string& PixelShader);
	//Shader(const std::string& commonData, const std::string& VertexShader, const std::string& PixelShader, const std::string& GeometryShader);

	~Shader();

	void Use() const { m_program.Use(); }
	GLint GetUniformLoc(const std::string& Uniform) const;

private:

	// Generic function to open a shader file
	std::string readFile(const std::string& ShaderPath);

	oglplus::VertexShader m_vertexShader;
	oglplus::FragmentShader m_fragmentShader;
	oglplus::GeometryShader m_geometryShader;
	oglplus::Program m_program;
};