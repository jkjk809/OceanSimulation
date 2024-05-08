#include "Screen.h"
#include "Keyboard.h"
#include "Mouse.h"

unsigned int Screen::SCR_WIDTH;
unsigned int Screen::SCR_HEIGHT;
glm::vec4 clearColor();
void Screen::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

Screen::Screen()
	: window(nullptr) {}

bool Screen::init()
{
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
	SCR_WIDTH = 2560;
	SCR_HEIGHT = 1440;

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simple Render", primaryMonitor, NULL);

	if (!window)
	{
		return false;
	}
	
	glfwMakeContextCurrent(window);
	return true;
}

void Screen::setParameters()
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	clearColor = glm::vec4(0.949f, 0.882f, 0.682f, 1.0f);
	glfwSetFramebufferSizeCallback(window, Screen::framebufferSizeCallback);

	//clearColor = glm::vec4(.631f, 0.553f, 0.66f, 1.0f);

	glfwSetKeyCallback(window, Keyboard::keyCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);
	glfwMakeContextCurrent(window);
	glEnable(GL_DEPTH_TEST);
}

void Screen::update()
{
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::newFrame()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool Screen::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Screen::setShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}

void Screen::setClearColor(float r, float g, float b, float a) {
	clearColor = glm::vec4(r, g, b, a);
}