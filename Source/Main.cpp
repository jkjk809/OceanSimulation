#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/Shader.h"
#include "io/Mouse.h"
#include "io/Keyboard.h"
#include "io/Screen.h"
#include "io/Camera.h"
#include "graphics/Texture.h"
#include "graphics/models/Cube.hpp"
#include "graphics/Model.h"

void processInput();

auto currentTime = std::chrono::steady_clock::now();
auto lastToggleTime = std::chrono::steady_clock::now();

float deltaTime = 0.0f;	
float lastFrame = 0.0f; 

Camera camera(glm::vec3(0.0f, 0.5f, 7.0f));
Screen screen;
Mouse mouse(&camera);
bool wireframe = false;
bool pause = false;
glm::vec3 lightPos(0.0f, 2.0f, 0.0f);


int main()
{
	stbi_set_flip_vertically_on_load(true);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	if (!screen.init())
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	screen.setParameters();

	Shader objectShader("Resources\\shader.vert", "Resources\\shader.frag");
	Shader lightShader("Resources\\shader.vert", "Resources\\light.frag");

	Model water(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.5f));
	water.loadModel("models/WATER2.obj");

	Cube lightCube(lightPos, glm::vec3(0.5f));
	lightCube.init();


	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), static_cast<float>(screen.SCR_WIDTH) / static_cast<float> (screen.SCR_HEIGHT), 0.1f, 100.0f);

	lightShader.use();
	lightShader.setMat4("projection", projection);
	
	objectShader.use();
	objectShader.setMat4("projection", projection);
	objectShader.setVec3("light.ambient", 0.7f, 0.7f, 0.7f);
	objectShader.setVec3("light.diffuse", 0.75f, 0.75f, 0.75f); // darken diffuse light a bit
	objectShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
	objectShader.setVec3("lightPos", lightPos);
	objectShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	objectShader.setFloat("material.shininess", 32.0f);
	objectShader.setFloat("light.constant", 1.0f);
	objectShader.setFloat("light.linear", 0.09f);
	objectShader.setFloat("light.quadratic", 0.032f);

	objectShader.setVec3("objectColor", 0.388, 0.831, 0.988);
	objectShader.setVec3("lightColor", 0.4f, 0.4f, 0.4f);
	
	float time = glfwGetTime();

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 2560, 1440);

	while (!screen.shouldClose())
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput();
		screen.update();
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, 2560, 1440);
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
		

		time = glfwGetTime();
		glm::mat4 view = camera.GetViewMatrix();

		objectShader.use();
		objectShader.setMat4("view", view);
		if (!pause) {
			objectShader.setFloat("time", time);
		}
		
		objectShader.setVec3("viewPos", camera.Position);

		water.render(objectShader);
		
		screen.newFrame();
	}
	water.cleanup();
	glDeleteVertexArrays;
	glfwTerminate();
	return 0;
	
}

void processInput()
{

	if (Keyboard::key(GLFW_KEY_ESCAPE))
		screen.setShouldClose(true);

	if (Keyboard::key(GLFW_KEY_M))
	{
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastToggleTime).count();
		if (elapsedTime > 200) // Cooldown period: 200 milliseconds
		{
			wireframe = !wireframe;
			lastToggleTime = currentTime;
		}
	}
	if (Keyboard::key(GLFW_KEY_P))
	{
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastToggleTime).count();
		if (elapsedTime > 200) // Cooldown period: 200 milliseconds
		{
			pause = !pause;
			lastToggleTime = currentTime;
		}
	}
	const float cameraSpeed = 3.0f * deltaTime; // adjust accordingly
	if (Keyboard::key(GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, cameraSpeed);

	if (Keyboard::key(GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, cameraSpeed);

	if (Keyboard::key(GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, cameraSpeed);

	if (Keyboard::key(GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, cameraSpeed);
	if(false)
	camera.Position.y = 1.0f;
}
