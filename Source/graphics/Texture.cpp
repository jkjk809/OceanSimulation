#include "Texture.h"
#include <iostream>


Texture::Texture() {};


Texture::Texture(std::string dir, std::string path, aiTextureType type)
	: dir(dir), path(path), type(type) {
	generate();
}

void Texture::generate()
{
	glGenTextures(1, &id);
}

void Texture::load(bool flip)
{
	stbi_set_flip_vertically_on_load(flip);
	
	int width, height, nChannels;
	unsigned char* data = stbi_load((dir + "/" + path).c_str(), &width, &height, &nChannels, 0);

	if (data)
	{
		GLenum format;
		if (nChannels == 1)
			format = GL_RED;
		else if (nChannels == 3)
			format = GL_RGB;
		else if (nChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}
	stbi_image_free(data);
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}