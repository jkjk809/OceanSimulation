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

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;  
uniform bool enableSpecular;
uniform Material material;
uniform bool lighting;
vec3 objColor = vec3(0.388, 0.831, 0.988);

void main()
{
   
   float distance = length(lightPos - FragPos);
   float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));    

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    vec3 ambient  = light.ambient * objectColor;
    vec3 diffuse  = light.diffuse * diff * objectColor;
    vec3 specular = light.specular * spec * vec3(1.0,1.0,1.0);
    diffuse *= attenuation;
    specular *= attenuation;
    vec3 result = (ambient + diffuse + specular) * objectColor ;
    
    FragColor = vec4(result, 1.0);
    
};