#ifndef FRAMEBUFFER_CLASS_H
#define FRAMEBUFFER_CLASS_H

#include <glad/glad.h>

#include "shader.h"
#include "Texture.h"
//#include "CubeMap.h"

#include "stb_image.h"

class FrameBuffer
{
public:

	//GLuint ID;
	unsigned int ID;
	unsigned int textureColorbufferID;
	unsigned int rboID;

	//constructor, used for shadow map
	//FrameBuffer(unsigned int shadowMapWidth = 2048, unsigned int shadowMapHeight = 2048);
	FrameBuffer::FrameBuffer(unsigned int noLight,bool forShadows);

	//constructor, bind an empty texture and  render buffer to the framebuffer
	FrameBuffer(unsigned int width = 800, unsigned int height = 600, unsigned int nbCol = 0);

	//constructor, bind an empty texture and  render buffer to the framebuffer
	FrameBuffer(Texture& texture, unsigned int width = 800, unsigned int height = 600);
	
	// Binds the framebuffer
	void Bind(GLuint unit);
	// Unbinds the framebuffer
	void Unbind();
	// Deletes the framebuffer
	void Delete();
	void attachTex2D(Texture& texture);

	void attachCubeFace(GLuint cubemapID, int iFace);
};
#endif
