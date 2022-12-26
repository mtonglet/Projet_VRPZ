#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>

#include "shader.h"


#include "stb_image.h"


class Texture
{
	
public:
	GLuint ID;
	//unit is the number of the texture for TEXTURE_0,...


	Texture(const char* image, const char* texType);

	// Binds a texture
	void Bind(GLuint unit);
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
 
	
};
#endif