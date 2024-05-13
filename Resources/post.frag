#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

out vec4 FragColor;

void main()
{
    // Sample the texture at the given coordinates
    vec4 texColor = texture(sceneTexture, TexCoords);

    // Output the color unchanged
    FragColor = texColor;
}