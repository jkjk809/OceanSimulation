#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imconfig.h"

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
float fps = 0.0f;

Camera camera(glm::vec3(0.0f, 5.0f, 0.0f));
Screen screen;
Mouse mouse(&camera);
bool wireframe = false;
bool pause = false;
bool day = true;
glm::vec3 lightPos(0.0f, 2.35f, 0.0f);

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
	Shader skyboxShader("Resources\\skybox.vert", "Resources\\skybox.frag");
	Shader postShader("Resources\\post.vert", "Resources\\post.frag");
	// ill probably make a cube map class later, idc rn tbh.
	float rectangleVertices[] =
	{
		// Coords    // texCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	unsigned int rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);

	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

	glBindVertexArray(rectVAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	float skyboxVertices[] =
	{
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,    
		-1.0f,  1.0f, -1.0f
	};

	unsigned int skyboxIndices[] =
	{
		// Right
		1, 2, 6,
		6, 5, 1,
		// Left
		0, 4, 7,
		7, 3, 0,
		// Top
		4, 5, 6,
		6, 7, 4,
		// Bottom
		0, 3, 2,
		2, 1, 0,
		// Back
		0, 1, 5,
		5, 4, 0,
		// Front
		3, 7, 6,
		6, 2, 3
	};

	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::string facesCubemap[6] =
	{
		"models\\textures\\night2.jpg",
		"models\\textures\\night2.jpg",
		"models\\textures\\night2.jpg",
		"models\\textures\\night2.jpg",
		"models\\textures\\black (1).jpg",
		"models\\textures\\night2.jpg",
	};
	std::string dayCubemap[6] =
	{
	    "models\\textures\\right.png",
		"models\\textures\\left.png",
		"models\\textures\\top.png",
		"models\\textures\\bottom.png",
		"models\\textures\\front.png",
		"models\\textures\\back.png",
	};
	unsigned int cubemapTexture, dayTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glGenTextures(1, &dayTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(dayCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << dayCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}


	Model water(glm::vec3(0.0f, 0.0f, -105.0f), glm::vec3(1.7f));
	water.loadModel("models/waterLarge.obj");

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), static_cast<float>(screen.SCR_WIDTH) / static_cast<float> (screen.SCR_HEIGHT), 0.1f, 1000.0f);

	skyboxShader.use();
	skyboxShader.setMat4("projection", projection);

	int nWaves = 50;
	float seed = 4336.460f;
	float initialAmp = 0.46f;
	float initialFreq = 0.44f;
	float initialSpeed = 1.54f;
	float ampIncrease = 0.69f;
	float freqIncrease = 1.24f;
	float speedIncrease = 0.95f;
	float fresnelFactor = 1.0f;
	float shiny = 128.0f;

	glm::vec3 fogColor = glm::vec3(1.0f, 0.89f, 0.745f);
	glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, 2.97f);
	glm::vec3 lightAmbient = glm::vec3(0.133f, 0.306f, 0.529f);
	glm::vec3 lightDiffuse = glm::vec3(0.749f, 0.925f, 1.0f);
	glm::vec3 lightSpecular = glm::vec3(1.0f, 0.737f, 0.471f);

	objectShader.use();

	objectShader.setInt("nWaves", nWaves);
	objectShader.setFloat("seed", seed);
	objectShader.setFloat("initialAmp", initialAmp);
	objectShader.setFloat("initialFreq", initialFreq);
	objectShader.setFloat("initialSpeed", initialSpeed);
	objectShader.setFloat("ampIncrease", ampIncrease);
	objectShader.setFloat("freqIncrease", freqIncrease);
	objectShader.setFloat("speedIncrease", speedIncrease);
	

	
	objectShader.setMat4("projection", projection);
	objectShader.setVec3("lightPos", lightPos);
	
	objectShader.setFloat("light.constant", 1.0f);
	objectShader.setFloat("light.linear", 0.045f);
	objectShader.setFloat("light.quadratic", 0.0075f);
	
	objectShader.setVec3("objectColor", 0.533f, 0.729f, 0.757f);
	objectShader.setVec3("lightColor", 1.0f, 0.525f, 0.0f);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	float time = glfwGetTime();

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 2560, 1440);

	postShader.use();
	postShader.setInt("screenTexture", 0);

	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screen.SCR_WIDTH, screen.SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	unsigned int textureColorBuffer;
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.SCR_WIDTH, screen.SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(screen.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	while (!screen.shouldClose())
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		fps = 1.0f / deltaTime;

		processInput();
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, screen.SCR_WIDTH, screen.SCR_HEIGHT);
		screen.update();
		
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (day)
		{
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexture);
			objectShader.use();
			objectShader.setInt("skybox", 4);
			skyboxShader.use();
			skyboxShader.setInt("skybox", 4);
		}
		else
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			objectShader.use();
			objectShader.setInt("skybox", 3);
			skyboxShader.use();
			skyboxShader.setInt("skybox", 3);
		}

		time = glfwGetTime();
		glm::mat4 view = camera.GetViewMatrix();
	

		objectShader.use();
		if (!pause) {
			objectShader.setFloat("time", time);
		}
		objectShader.setMat4("view", view);
		objectShader.setVec3("viewPos", camera.Position);

		objectShader.setInt("nWaves", nWaves);
		objectShader.setFloat("seed", seed);
		objectShader.setFloat("initialAmp", initialAmp);
		objectShader.setFloat("initialFreq", initialFreq);
		objectShader.setFloat("initialSpeed", initialSpeed);
		objectShader.setFloat("ampIncrease", ampIncrease);
		objectShader.setFloat("freqIncrease", freqIncrease);
		objectShader.setFloat("speedIncrease", speedIncrease);

		objectShader.setVec3("fogColor", fogColor);
		objectShader.setVec3("light.direction", lightDirection);
		objectShader.setVec3("light.ambient", lightAmbient);
		objectShader.setVec3("light.diffuse", lightDiffuse);
		objectShader.setVec3("material.specular", lightSpecular);
		objectShader.setVec3("material.specular", lightSpecular);
		objectShader.setFloat("fresnelFactor", fresnelFactor);
		objectShader.setFloat("shiny", shiny);

		water.render(objectShader);
		
		glDepthFunc(GL_LEQUAL);

		skyboxShader.use();

		//removes perspective
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.setMat4("view", view);

		glBindVertexArray(skyboxVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, screen.SCR_WIDTH, screen.SCR_HEIGHT);
		postShader.use();

		glBindVertexArray(rectVAO);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Ocean Simulation");
		ImGui::Text("FPS: %.1f", fps);
		if (ImGui::CollapsingHeader("Ocean Parameters"))
		{
			ImGui::SliderInt("Number of Waves", &nWaves, 1, 128);
			ImGui::DragFloat("Wave Seed", &seed, 0.01f);
			ImGui::InputFloat("Input Wave Seed", &seed);
			ImGui::DragFloat("Initial Amplitude", &initialAmp, 0.01f);
			ImGui::DragFloat("Increase Amplitude", &ampIncrease, 0.01f);
			ImGui::DragFloat("Initial Frequency", &initialFreq, 0.01f);
			ImGui::DragFloat("Increase Frequency", &freqIncrease, 0.01f);
			ImGui::DragFloat("Initial Speed", &initialSpeed, 0.01f);
			ImGui::DragFloat("Increase Speed", &speedIncrease, 0.01f);
		}

		if (ImGui::CollapsingHeader("Lighting"))
		{
			ImGui::ColorEdit3("fogColor", &fogColor[0]);
			ImGui::SliderFloat3("Light Direction", &lightDirection[0], -10.0f, 10.0f, "%.6f");
			ImGui::ColorEdit3("Ambient Light", &lightAmbient[0]);
			ImGui::ColorEdit3("Diffuse Light", &lightDiffuse[0]);
			ImGui::ColorEdit3("Specular Light", &lightSpecular[0]);
			ImGui::DragFloat("Shininess", &shiny, 0.1f);
			ImGui::DragFloat("Fresnel Factor", &fresnelFactor, 0.01f);
			
		}

		if (ImGui::CollapsingHeader("Change environment"))
		{
			if (ImGui::Button("Day"))
			{
				day = true;
				fogColor = glm::vec3(1.0f, 0.89f, 0.745f);
				lightAmbient = glm::vec3(0.133f, 0.306f, 0.529f);
				lightDiffuse = glm::vec3(0.749f, 0.925f, 1.0f);
				lightSpecular = glm::vec3(1.0f, 0.737f, 0.471f);
			}
			if (ImGui::Button("Night"))
			{
				day = false;
				fogColor = glm::vec3(0.106f, 0.157f, 0.2f);
				lightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
				lightDiffuse = glm::vec3(0.1f, 0.1f, 0.1f);
				lightSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
			}
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
	const float cameraSpeed = 10.0f * deltaTime; // adjust accordingly
	if (Keyboard::key(GLFW_KEY_W) == GLFW_PRESS)
		camera.Position.y += 0.2f;

	if (Keyboard::key(GLFW_KEY_S) == GLFW_PRESS)
		camera.Position.y -= 0.2f;
	if (Keyboard::key(GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, cameraSpeed);

	if (Keyboard::key(GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, cameraSpeed);
	if(false)
	camera.Position.y = 1.0f;
}

	