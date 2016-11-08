#ifdef _DEBUG
	#include "vld.h"
#endif

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <oglplus/config/site.hpp>

#include <log.h>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "Skybox.h"
#include "HUDText.h"
#include "Timer.h"
#include "Renderer.h"
#include "ScreenQuad.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();

// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;

// Camera
Camera  camera(glm::vec3(0.0f, 2.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main() {
	FILELog::ReportingLevel() = logDEBUG;
	FILE* log_fd = fopen("log.txt", "w");
	Output2FILE::Stream() = log_fd;

	// Init GLFW
	if (!glfwInit()) {
		throw std::runtime_error("GLFW initialization error.");
	}

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OGLPLUS_GL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OGLPLUS_GL_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 1);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Start up OpenGL renderer
	Renderer renderer(1280, 720);

	ScreenQuad screenQuad;

	// Shaders
	Shader floorShader("shaders/common.vert", "shaders/floorvs.glsl", "shaders/floorps.glsl");
	Shader shader("shaders/common.vert", "shaders/nanosuitvs.glsl", "shaders/nanosuitps.glsl");
	Shader lightShader("shaders/common.vert", "shaders/lampvs.glsl", "shaders/lampps.glsl");
	Shader skyboxShader("", "shaders/skyboxvs.glsl", "shaders/skyboxps.glsl");
	Shader fontShader("", "shaders/fontvs.glsl", "shaders/fontps.glsl");

	Shader geometryPassShader("", "shaders/deferredGeometryPassVS.glsl", "shaders/deferredGeometryPassPS.glsl");
	Shader lightingPassShader("", "shaders/deferredLightPassVS.glsl", "shaders/deferredLightPassPS.glsl");

	// Set samplers
	lightingPassShader.Use();
	glUniform1i(lightingPassShader.GetUniformLoc("gPosition"), 0);
	glUniform1i(lightingPassShader.GetUniformLoc("gNormal"), 1);
	glUniform1i(lightingPassShader.GetUniformLoc("gAlbedoSpec"), 2);

	std::vector<glm::vec3> objectPositions;
	objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

	// - Colors
	const GLuint NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	srand(13);
	for (GLuint i = 0; i < NR_LIGHTS; ++i) {
		// Calculate slightly random offsets
		GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	HUDText debugText(fontShader, "fonts/arial.ttf", WIDTH, HEIGHT);
	Skybox skybox("skybox/ocean/");
	Light light(glm::vec3(2.3f, 2.0f, -3.0f), glm::vec3(1.0f), Light::POINTLIGHT);
	Model nanosuit("models/nanosuit/nanosuit.obj");
	Model floor("models/floor/3d-model.obj");
	floor.SetInstancing( { glm::vec3(0.0f), glm::vec3(-14.575f, 0.0f, 0.0f), glm::vec3(14.575f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 14.575f) } );

	// Set up G-Buffer
	// 3 textures:
	// 1. Positions (RGB)
	// 2. Color (RGB) + Specular (A)
	// 3. Normals (RGB) 
	GLuint gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	GLuint gPosition, gNormal, gAlbedoSpec;
	
	// - Position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	
	// - Normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	
	// - Color + Specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	
	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	
	// - Create and attach depth buffer (renderbuffer)
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Renderer::ClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Update loop
	while (!glfwWindowShouldClose(window)) {
		
		renderer.PreRender(glfwGetTime());
		// Check for any events (key pressed, mouse moved etc.).
		glfwPollEvents();
		do_movement();

		// Geometry pass
		// 1. Geometry Pass: render scene's geometry/color data into gbuffer
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(camera.GetFOV(), static_cast<GLfloat>(WIDTH) / static_cast<GLfloat>(HEIGHT), 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrixGL();
		glm::mat4 model;

		geometryPassShader.Use();
		glUniformMatrix4fv(geometryPassShader.GetUniformLoc("projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(geometryPassShader.GetUniformLoc("view"), 1, GL_FALSE, glm::value_ptr(view));
		for (GLuint i = 0; i < 1; ++i) {
			model = glm::mat4();
			model = glm::translate(model, objectPositions.at(i));
			model = glm::scale(model, glm::vec3(0.25f));
			glUniformMatrix4fv(geometryPassShader.GetUniformLoc("model"), 1, GL_FALSE, glm::value_ptr(model));
			nanosuit.Draw(geometryPassShader);
		}

		model = glm::mat4();
		model = glm::scale(model, glm::vec3(0.3f, 0.2f, 0.3f));
		glUniformMatrix4fv(geometryPassShader.GetUniformLoc("model"), 1, GL_FALSE, value_ptr(model));
		floor.DrawInstanced(geometryPassShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Lighting Pass

		//oglplus::Texture::Active(3);
		//glUniform1i(lightingPassShader.GetUniformLoc("skybox"), 3);
		//skybox.BindTexture();
		/// DRAW INSTANCED
		// 2. Lighting Pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightingPassShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		// Also send light relevant uniforms
		for (GLuint i = 0; i < lightPositions.size(); i++)
		{
			glUniform3fv(lightingPassShader.GetUniformLoc("lights[" + std::to_string(i) + "].Position"), 1, &lightPositions[i][0]);
			glUniform3fv(lightingPassShader.GetUniformLoc("lights[" + std::to_string(i) + "].Color"), 1, &lightColors[i][0]);
			// Update attenuation parameters and calculate radius
			const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
			const GLfloat linear = 0.7;
			const GLfloat quadratic = 1.8;
			glUniform1f(lightingPassShader.GetUniformLoc("lights[" + std::to_string(i) + "].Linear"), linear);
			glUniform1f(lightingPassShader.GetUniformLoc("lights[" + std::to_string(i) + "].Quadratic"), quadratic);
		}
		glUniform3fv(lightingPassShader.GetUniformLoc("viewPos"), 1, &camera.GetPosition()[0]);

		//skybox.Draw(camera.GetViewMatrixGL(), projection);
		// Disable depth test for screen quad and HUD
		Renderer::DisableCapability(oglplus::Capability::DepthTest);

		// Render post-processed quad
		screenQuad.Draw();

		// Draw Text on top of everything
		debugText.RenderText(fontShader, std::to_string(renderer.GetFPS()) + " fps", 0.0f, HEIGHT - 36.0f, 1.0f, glm::vec3(1.0f));
		
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement() {
	// Camera controls
	if (keys[GLFW_KEY_W]) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
		std::cout << "w\n";
	}
	if (keys[GLFW_KEY_S]) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		std::cout << "s\n";
	}
	if (keys[GLFW_KEY_A]) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D]) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}