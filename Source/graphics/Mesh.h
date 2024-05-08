#pragma once
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normals;
	
	static std::vector<struct Vertex> genList(float* vertices, int nVertices);
};
typedef struct Vertex Vertex;

class Mesh
{
public:

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO;
	std::vector<Texture> textures;

	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures = {});

	void render(Shader shader);

	void cleanup();

private:
	unsigned int VBO, EBO;
	
	void setup();

};

#endif