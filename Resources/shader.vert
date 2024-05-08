#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
float amplitude = 0.3f;
float wavelength = 0.5f;
float frequency = 2.0f/wavelength;
float speed = 0.8 * frequency;
//f(x,t) = amp * sin(x * frequency + t * speed)

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

float displaceVertex(vec3 worldPos)
{
	float totalWave = 0.0f;

		// do it by hand for now who cares
		float waveOffset = amplitude * exp(sin(worldPos.x * frequency + (worldPos.z * frequency) + time * speed)) - 1.0;
		float waveOffset2 = 0.17 * exp(sin(worldPos.x * 3.3 + (worldPos.z * 1.4) + time * 2.8f)) - 1.0;
		float waveOffset3 = 0.24 * exp(sin(worldPos.x * 2.3 + (worldPos.z * 2.3) + time * 1.84f)) - 1.0;
		totalWave = waveOffset + waveOffset2 + waveOffset3;

	return worldPos.y + amplitude * totalWave;
}

void main()
{
   //position of our vertices in world coordinates
   vec4 worldPos = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   //normals of our vertices
   Normal = mat3(transpose(inverse(model))) * aNormal;
   TexCoords = aTexCoords;

   vec3 displacedPos = vec3(worldPos.x, displaceVertex(worldPos.xyz), worldPos.z);

   FragPos = displacedPos;

   gl_Position = projection * view * model * vec4(FragPos, 1.0);
   
};
