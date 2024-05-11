#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

uniform int nWaves;
uniform float seed;
uniform float initialAmp;
uniform float initialFreq;
uniform float initialSpeed;

uniform float ampIncrease;
uniform float freqIncrease;
uniform float speedIncrease;
//f(x,t) = amp * sin(x * frequency + t * speed)

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;


	

vec3 totalNormals = vec3(0.0f,0.0f,0.0f);

float random (vec2 seed) 
{
    return fract(sin(dot(seed.xy,vec2(12.9898,78.233))) * 43758.5453123);
}

vec2 getRandomDirection(int waveNum)
{
    vec2 dir = vec2(1);
    dir.x = 2.0 * random(vec2(waveNum, seed)) - 1.0;
    dir.y = 2.0 * random(vec2(waveNum, seed + 1)) - 1.0;
    return normalize(dir);
}

vec3 calcNormals(float dx,float dz){

	vec3 tangentVector = vec3(1.0, dx, 0.0);  
    vec3 binormalVector = vec3(0.0, dz, 1.0);

    vec3 normalVector = normalize(cross(binormalVector, tangentVector));
    return normalVector;
}

float displaceVertex(vec3 worldPos)
{
	float totalWave = 0.0f;
	float amplitude = initialAmp;
	float frequency = initialFreq;
	float speed = initialSpeed;
	float dx = 0.0f;
    float dz = 0.0f;
	float lastDX = 0.0f;
	float lastDZ = 0.0f;

	for(int i = 0; i < nWaves; i++)
	{
		worldPos.x += lastDX * 0.02;
		worldPos.z += lastDZ * 0.02;
		vec2 direction = getRandomDirection(i);
		

		float waveOffset = amplitude * exp(sin((dot(direction, worldPos.xz) * frequency) + time * speed) - 1.0);
		float wavePhase = dot(direction, worldPos.xz) * frequency + time * speed;
		float sinWave = sin(wavePhase);
		float cosWave = cos(wavePhase);
		float expWave = exp((sinWave));

		totalWave += waveOffset;

		dx += amplitude * frequency * expWave * direction.x * cosWave;
        dz += amplitude * frequency * expWave * direction.y * cosWave;
	    lastDX = dx;
	    lastDZ = dz;
		amplitude *= ampIncrease;
		frequency *= freqIncrease;
		speed *= speedIncrease;
		
	}
	totalNormals = calcNormals(dx, dz);
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
