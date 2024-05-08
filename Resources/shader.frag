#version 330 core
//in vec3 color;

out vec4 FragColor;
layout(location = 0) out vec3 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;  
uniform vec3 viewPos;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;  
uniform bool enableSpecular;
uniform Material material;
uniform bool lighting;
vec3 objColor = vec3(0.388, 0.831, 0.988);

void main()
{
// vec2 correctedTexCoord = TexCoords + vec2(FragPos.x * Depth, FragPos.y * Depth);
   
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
     vec3 ambient  = light.ambient * objectColor;
    vec3 diffuse  = light.diffuse * diff * objectColor;

    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
    
};