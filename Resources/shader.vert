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
vec3 directionVectorX = vec3(1,0,0);
vec3 directionVectorZ = vec3(0,0,1);
//f(x,t) = amp * sin(x * frequency + t * speed)

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
vec3 totalNormals;

vec3 calcNormals(vec3 worldPos){
	float partialX = amplitude * frequency * exp(sin(worldPos.x * frequency + worldPos.z * frequency + time * speed)-1.0) * cos(worldPos.x * frequency + worldPos.x * frequency + time * speed);
	float partialZ = amplitude * frequency * exp(sin(worldPos.x * frequency + worldPos.z * frequency + time * speed)-1.0) * cos(worldPos.z * frequency + worldPos.x * frequency + time * speed);
	//float partialX2 = 0.17 * 3.3 * exp(sin(worldPos.x * 3.3  + time * 2.8f) - 1.0) * cos(worldPos.x * 3.3 + time * 2.8f);
	//float partialZ2 = 0.17 * 3.3 * exp(sin(worldPos.z * 3.3 + time * 2.8f) - 1.0) * cos(worldPos.z * 3.3 + time * 2.8f);
	//float partialX3 = 0.24 * 2.3 * exp(sin(worldPos.x * 2.3 + time * 1.84f) - 1.0) * cos(worldPos.x * 2.3 + time * 1.84f);
	//float partialZ3 = 0.24 * 2.3 * exp(sin(worldPos.z * 2.3 + time * 1.84f) - 1.0) * cos(worldPos.z * 2.3 + time * 1.84f);

	vec3 tangentVector = vec3(1.0f, partialX, 0.0f);
	vec3 binormalVector = vec3(0.0f, partialZ, 1.0f);
	vec3 normalVector = cross(binormalVector, tangentVector);
	
	return normalVector;
}

float displaceVertex(vec3 worldPos)
{
	float totalWave = 0.0f;

		// do it by hand for now who cares
		//float waveOffset = sin(worldPos.x + time);
		float waveOffset = amplitude * sin(worldPos.x * frequency + time * speed) + sin(worldPos.z * frequency + time * speed); 
	//	float waveOffset2 = 0.17 * exp(sin(worldPos.x * 3.3 + (worldPos.z * 1.4) + time * 2.8f)) - 1.0;
	//	float waveOffset3 = 0.24 * exp(sin(worldPos.x * 2.3 + (worldPos.z * 2.3) + time * 1.84f)) - 1.0;
		totalWave = waveOffset;

	return worldPos.y  + amplitude * totalWave;
}



void main()
{
   //position of our vertices in world coordinates
   vec4 worldPos = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   //normals of our vertices
   TexCoords = aTexCoords;

   vec3 displacedPos = vec3(worldPos.x, displaceVertex(worldPos.xyz), worldPos.z);

   vec3 normalVector = calcNormals(worldPos.xyz);
   Normal = normalVector;

   FragPos = displacedPos.xyz;

   gl_Position = projection * view * model * vec4(FragPos, 1.0);
   
};
