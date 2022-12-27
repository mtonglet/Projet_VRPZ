#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <glad/glad.h>

#include "shader.h"


#include "stb_image.h"


class Texture
{
	
public:
	GLuint ID;
	//unit is the number of the texture for TEXTURE_0,...


	Texture(const char* image, const char* texType);

	Texture(int width = 800, int heigth = 600, int numColCh = 0);

	// Binds a texture
	void Bind(GLuint unit);
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
 
	
};
#endif