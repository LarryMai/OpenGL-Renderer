#include "Shader.h"

#include <oglplus/uniform.hpp>

#include <log.h>
#include <fstream>
#include <sstream>

/***********************************************************************************/
Shader::Shader(const std::string& commonData, const std::string& VertexShader, const std::string& PixelShader) {
	
	std::string vertexCode;

	// Is the shader using common data?
	if (!commonData.empty()) {
		vertexCode = readFile(commonData);
		// Retrieve the vertex/fragment source code from filePath
		vertexCode.append(readFile(VertexShader));
	} else {
		vertexCode = readFile(VertexShader);
	}

	auto fragmentCode = readFile(PixelShader);

	m_vertexShader.Source(vertexCode).Compile();
	m_fragmentShader.Source(fragmentCode).Compile();

	m_program.AttachShader(m_vertexShader).AttachShader(m_fragmentShader);
	m_program.Link();
}

/***********************************************************************************
Shader::Shader(const std::string& commonData, const std::string& VertexShader, const std::string& PixelShader, const std::string& GeometryShader) {
	// Retrieve the vertex/fragment source code from filePath
	auto vertexCode = readFile(VertexShader);
	auto fragmentCode = readFile(PixelShader);
	auto geometryCode = readFile(GeometryShader);
}
*/

/***********************************************************************************/
Shader::~Shader() {
}

/***********************************************************************************/
GLint Shader::GetUniformLoc(const std::string& Uniform) const {
	GLint loc = glGetUniformLocation(oglplus::GetGLName(m_program), Uniform.c_str());
	if (loc == -1) {
		std::string error = "Uniform: " + Uniform + " does not exist.\n";
		std::cerr << error;
	}
	return loc;
}

/***********************************************************************************/
std::string Shader::readFile(const std::string& ShaderPath) {
	std::string shaderCode;
	std::ifstream file;
	std::stringstream fileSS;

	file.exceptions(std::ifstream::badbit);
	try {
		// Open files
		file.open(ShaderPath);
		if (file) {
			// Read file's buffer contents into streams
			fileSS << file.rdbuf();
			// close file handlers
			file.close();
			// Convert stream into string
			shaderCode = fileSS.str();

			return shaderCode;
		} 
		throw std::runtime_error("Failed to open: " + ShaderPath + '\n');
	}
	catch (const std::ifstream::failure& e) {
		std::cerr << "Shader error: " << e.what();
		FILE_LOG(logERROR) << "Shader error " << e.what();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Shader error: " << e.what();
		FILE_LOG(logERROR) << "Shader error " << e.what();
	}
	catch (...) {
		std::cout << "Unhandled exception in Shader::readShader().\n";
	}
}