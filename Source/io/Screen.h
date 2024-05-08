#pragma once
#ifndef SCREEN_H
#define SCREEN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Screen
{
public:
	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	Screen();

	bool init();

	void setParameters();
	void setClearColor(float r, float g, float b, float a);
	void update();
	void newFrame();

	bool shouldClose();
	void setShouldClose(bool shouldClose);

private:
	GLFWwindow* window;
	glm::vec4 clearColor;
};


#endif