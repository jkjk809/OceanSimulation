#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

float directionSeed = 74.0f;

//f(x,t) = amp * sin(x * frequency + t * speed)

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;


	

vec3 totalNormals = aNormal;

float random (vec2 seed) 
{
    return fract(sin(dot(seed.xy,vec2(12.9898,78.233))) * 43758.5453123);
}

vec2 getRandomDirection(int waveNum)
{
    vec2 dir = vec2(1);
    dir.x = 2.0 * random(vec2(waveNum, directionSeed)) - 1.0;
    dir.y = 2.0 * random(vec2(waveNum, directionSeed + 1)) - 1.0;
    return normalize(dir);
}

vec3 calcNormals(float expWave, float cosWave, float amplitude, float frequency, vec2 direction){
	float dx = amplitude * frequency * expWave * direction.x * cosWave;
    float dz = amplitude * frequency * expWave * direction.y * cosWave;

	vec3 tangentVector = vec3(1.0, dx, 0.0);  
    vec3 binormalVector = vec3(0.0, dz, 1.0);

    vec3 normalVector = normalize(cross(binormalVector, tangentVector));
    return normalVector;
}

float displaceVertex(vec3 worldPos)
{
	float totalWave = 0.0f;
	float amplitude = 0.43f;
	float frequency = 0.8f;
	float speed = 1.2;

	for(int i = 0; i < 15; i++)
	{
	vec2 direction = getRandomDirection(i);
		
		float waveOffset = amplitude * exp(sin((dot(direction, worldPos.xz) * frequency) + time * speed) - 1.0);
		float wavePhase = dot(direction, worldPos.xz) * frequency + time * speed;
		float sinWave = sin(wavePhase);
		float cosWave = cos(wavePhase);
		float expWave = exp(sinWave - 1.0f);

		totalWave += amplitude * expWave;
		totalNormals += calcNormals(expWave, cosWave, amplitude, frequency, direction);

		amplitude *= 0.89;
		frequency *= 1.16;
		speed *= 1.07;
		
	}
	worldPos.y = totalWave;
	return worldPos.y;
}

void main()
{

   vec4 worldPos = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);

   TexCoords = aTexCoords;

   vec3 displacedPos = vec3(worldPos.x, displaceVertex(worldPos.xyz), worldPos.z);

   Normal = normalize(totalNormals);
   FragPos = displacedPos.xyz;
   gl_Position = projection * view * model * vec4(FragPos, 1.0);
   
};
