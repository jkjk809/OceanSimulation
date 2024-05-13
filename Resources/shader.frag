#version 330 core
//in vec3 color;

out vec4 FragColor;
layout(location = 0) out vec3 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform samplerCube skybox;
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

// for some reason material.shininess is not working for setting uniform idk.
uniform float shiny;
uniform Light light;  
uniform Material material;
uniform vec3 fogColor;
uniform float fresnelFactor;
//vec3 objColor = vec3(0.075f, 0.541f, 0.71f);
vec3 objColor = vec3(0.02f, 0.125f, 0.412f);

float near = 0.007f; 
float far  = 9.5f; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


float fresnel(vec3 viewDir, vec3 normal, float refIndex) {
    float cosTheta = dot(-viewDir, normal);
    return pow(1.0 - cosTheta, refIndex);
}

void main()
{
   float depth = LinearizeDepth(gl_FragCoord.z) / far;

   float distance = length(lightPos - FragPos);
  //for point lights
  // float attenuation = 1.0 / (light.constant + light.linear * distance + 
 // 		    light.quadratic * (distance * distance));    

    vec3 norm = normalize(Normal);
    
    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));
    float fresnel = pow(1.0 - max(dot(viewDir, norm), 0.05), fresnelFactor);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shiny);

    vec3 ambient  = light.ambient * objectColor;
    vec3 diffuse  = light.diffuse * diff * objectColor;
    vec3 specular = material.specular * spec * fresnel;

    //refraction, reflection didnt look good, ill probably add it at a later date.
    float ratio = 1.00 / 1.33;
    vec3 incidentRay = normalize(FragPos - viewPos);
    vec3 refractedRay = refract(incidentRay, normalize(Normal), ratio);
    vec3 refraction = (1.0 - fresnel) * texture(skybox, refractedRay).rgb;

    vec3 result = (ambient + diffuse + specular + refraction);
    vec3 finalColor = mix(result.rgb, fogColor, depth);
    FragColor = vec4(finalColor.rgb, 1.0);

};