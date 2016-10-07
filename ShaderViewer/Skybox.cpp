#include "Skybox.h"
#include "Texture.h"
#include "Renderer.h"

// Easier to use TS library than hard-code texture files
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

/***********************************************************************************/
Skybox::Skybox(const std::string& TextureDirectory) : m_shader("", "shaders/skyboxvs.glsl", "shaders/skyboxps.glsl") {

	m_vao.Bind();
	m_vbo.Bind(oglplus::Buffer::Target::Array);
	oglplus::Buffer::Data(oglplus::Buffer::Target::Array, m_skyboxVertices);

	oglplus::VertexArrayAttrib vert_attr(0);
	vert_attr.Setup<oglplus::Vec3f>().Enable();

	// Iterate through given directory and find files (labeled 1-6 for proper load order)
	int i = 0; // Index for the loop
	for (auto& it : fs::directory_iterator(TextureDirectory)) {
		m_faces.at(i) = it.path().generic_string(); // put texture paths into std::array for loading
		++i;
	}

	// Generate textures
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	
	unsigned char* image;
	int x, y;
	int n = 3; //Number of components to load (RGB)
	i = 0;
	for (auto& face : m_faces) {
		image = Texture::LoadSTBImage(face.c_str(), &x, &y, &n, 3); // 3 = STB_rgb
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		std::cout << "Loaded skybox:" << face << '\n';
		glGenerateMipmap(GL_TEXTURE_2D);
		free(image); // Cleanup
		++i;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	// Anisotropic filtering
	GLfloat aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

	// Cleanup
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

/***********************************************************************************/
Skybox::~Skybox() {
}

/***********************************************************************************/
void Skybox::Draw(const glm::mat4& CameraMatrix, const glm::mat4& ProjectionMat) {
	// Change depth function so depth test passes when values are equal to depth Buffer's content
	Renderer::SetDepthFunc(oglplus::CompareFunction::LEqual);
	
	m_shader.Use();
	// Transformations
	glm::mat4 view = glm::mat4(glm::mat3(CameraMatrix)); // Remove any translation component of the view matrix	
	glUniformMatrix4fv(m_shader.GetUniformLoc("view"), 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(m_shader.GetUniformLoc("projection"), 1, GL_FALSE, value_ptr(ProjectionMat));
	
	// Skybox Cube
	m_vao.Bind();
	oglplus::Texture::Active(0);

	glUniform1i(m_shader.GetUniformLoc("skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	Renderer::DrawArrays(oglplus::PrimitiveType::Triangles, 0, 36);
	
	// Don't change depth function to default since we will disable it later anyways.
}
