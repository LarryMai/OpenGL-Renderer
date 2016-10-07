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
#include "FrameBuffer.h"
#include "PostProcess.h"

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
		throw std::runtime_error("GLFW initialization error");
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
	Renderer renderer(1280, 720, Renderer::RenderMode::FILLED);

	// Create uniform buffer object for projection and view matrices (same data shared to multiple shaders)
	GLuint uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // Allocate memory, but do not fill
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// Define range of the buffer that links to a binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

	// Projection matrix does not change at runtime (constant window size)
	glm::mat4 projection = glm::perspective(camera.GetFOV(), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	// Insert data into allocated memory block
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	FrameBuffer fb(WIDTH, HEIGHT, false, false);
	PostProcess pp("shaders/screenQuadVert.glsl", "shaders/screenQuadPixel.glsl");

	// Shaders
	Shader floorShader("shaders/common.vert", "shaders/floorvs.glsl", "shaders/floorps.glsl");
	Shader shader("shaders/common.vert", "shaders/nanosuitvs.glsl", "shaders/nanosuitps.glsl");
	Shader lightShader("shaders/common.vert", "shaders/lampvs.glsl", "shaders/lampps.glsl");
	Shader skyboxShader("", "shaders/skyboxvs.glsl", "shaders/skyboxps.glsl");
	Shader fontShader("", "shaders/fontvs.glsl", "shaders/fontps.glsl");

	HUDText debugText(fontShader, "fonts/arial.ttf", WIDTH, HEIGHT);
	Skybox skybox("skybox/ocean/");
	Light light(glm::vec3(2.3f, 2.0f, -3.0f), glm::vec3(1.0f), Light::POINTLIGHT);
	Model nanosuit("models/nanosuit/nanosuit.obj");
	Model floor("models/floor/3d-model.obj");
	floor.SetInstancing( { glm::vec3(0.0f), glm::vec3(-14.575f, 0.0f, 0.0f), glm::vec3(14.575f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 14.575f) } );

	// Game loop
	while (!glfwWindowShouldClose(window)) {

		renderer.PreRender(glfwGetTime());
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();

		fb.Bind();
		// Render to bound framebuffer
		{
			// Clear the colorbuffer
			Renderer::ClearColor(0.3f, 0.3, 0.3, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
			// Enable depth testing for 3D stuff
			Renderer::EnableCapability(oglplus::Capability::DepthTest);
			
			// Transformations
			glm::mat4 view = camera.GetViewMatrixGL();
			glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
			
			light.Draw(lightShader);
			
			shader.Use();
			glUniform3f(shader.GetUniformLoc("viewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
			// We already have 3 texture units active (in this shader) so set the skybox as the 4th texture unit (texture units are 0 based so index number 3)
			glActiveTexture(GL_TEXTURE3);
			glUniform1i(shader.GetUniformLoc("skybox"), 3);
			skybox.BindTexture();
	
			// Draw loaded models
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(0.0f, 0.175f, 0.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			glUniformMatrix4fv(shader.GetUniformLoc("model"), 1, GL_FALSE, value_ptr(model));
			nanosuit.Draw(shader);
	
			floorShader.Use();
			model = glm::mat4();
			model = glm::scale(model, glm::vec3(0.3f, 0.2f, 0.3f));
			glUniformMatrix4fv(shader.GetUniformLoc("model"), 1, GL_FALSE, value_ptr(model));
			floor.DrawInstanced(floorShader);
	
			//Always draw skybox last
			skybox.Draw(camera.GetViewMatrixGL(), projection);
		}
		fb.UnBind(); 
		// Switch back to default framebuffer

		Renderer::ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// Disable depth test for screen quad and HUD
		Renderer::DisableCapability(oglplus::Capability::DepthTest);

		// Render post-processed quad
		pp.RendertoScreen(fb);

		// Draw Text on top of everything
		debugText.RenderText(fontShader, std::to_string(renderer.GetFPS()) + " fps", 0.0f, HEIGHT - 36.0f, 1.0f, glm::vec3(0.0f));
		
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