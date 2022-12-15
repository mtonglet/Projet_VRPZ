#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>

#include "shader.h"


#include "stb_image.h"

class Texture
{
public:
	GLuint ID;
	GLenum type;
	//unit is the number of the texture for TEXTURE_0,...
	GLuint unit;

	Texture(const char* image, GLenum texType, GLuint slot);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit );
	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};
#endif