#ifndef FRAMEBUFFER_CLASS_H
#define FRAMEBUFFER_CLASS_H

#include <glad/glad.h>

#include "shader.h"
#include "Texture.h"

#include "stb_image.h"

class FrameBuffer
{
public:

	//GLuint ID;
	unsigned int ID;
	unsigned int textureColorbufferID;
	unsigned int rboID;
	GLuint unit; //unit of the texture, consider creating textures 
	//constructor, bind an empty texture and  render buffer to the framebuffer
	FrameBuffer(unsigned int width = 800, unsigned int height = 600, unsigned int nbCol = 0);

	
	// Binds the framebuffer
	void Bind();
	// Unbinds the framebuffer
	void Unbind();
	// Deletes the framebuffer
	void Delete();
};
#endif
