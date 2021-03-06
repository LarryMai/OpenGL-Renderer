#include "GLShader.h"
#include "../ResourceManager.h"

#include <iostream>


/***********************************************************************************/
GLShader::GLShader(const std::string& shaderCode, const ShaderType type) {
	
	m_shaderID = glCreateShader(static_cast<decltype(0x8B30)>(type));

	try {
		compile(shaderCode.c_str());
	}
	catch (const std::runtime_error& err) {
		std::cerr << "Shader Error: " << m_shaderID << ". " << err.what() << '\n';
	}
}

/***********************************************************************************/
void GLShader::compile(const GLchar* shaderCode) {
	
	glShaderSource(m_shaderID, 1, &shaderCode, nullptr);
	glCompileShader(m_shaderID);
	
	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &m_success);
	if (!m_success) {
		glGetShaderInfoLog(m_shaderID, m_infoLog.size(), nullptr, m_infoLog.data());
		throw std::runtime_error(m_infoLog.data());
	}
}