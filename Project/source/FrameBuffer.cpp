#include "../headers/FrameBuffer.h"


ShadowFrameBuffer::ShadowFrameBuffer(unsigned int mapWidth, unsigned int mapHeight) {
	this->mapWidth = mapWidth;
	this->mapHeight= mapHeight;

	glGenFramebuffers(1, &ID);
	glGenTextures(1, &shadowMapTex);
	glBindTexture(GL_TEXTURE_2D, shadowMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mapWidth, mapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };// Prevents darkness outside the frustrum
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTex, 0);
	glDrawBuffer(GL_NONE);// Needed since we don't touch the color buffer
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowFrameBuffer::Bind(GLuint unit) {
	/*
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, mapWidth, mapHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glClear(GL_DEPTH_BUFFER_BIT);
	*/
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, shadowMapTex);
//	glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadowMap"), 2);

}

void ShadowFrameBuffer::Unbind(int winWidth, int winHeight) {
	// Switch back to the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Switch back to the default viewport
	glViewport(0, 0, winWidth, winHeight);
}


FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, unsigned int nbCol) {

	//creating the framebuffer
	
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	//creating the texture that goes with the framebuffer
	// generate texture
	
	glGenTextures(1, &textureColorbufferID);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureColorbufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, nbCol, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbufferID, 0);

	//create a render buffer for the depth and stencil tests
	
	glGenRenderbuffers(1, &rboID);
	glBindRenderbuffer(GL_RENDERBUFFER, rboID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//attach it to the frame buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID);

	//verify all went right and unbind the framebuffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//bind to the default framebuffer

}

FrameBuffer::FrameBuffer(Texture& texture, unsigned int width, unsigned int height, bool forShadows) {

	//creating the framebuffer

	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.ID, 0);

	//create a render buffer for the depth and stencil tests

	glGenRenderbuffers(1, &rboID);
	glBindRenderbuffer(GL_RENDERBUFFER, rboID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//attach it to the frame buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID);

	//verify all went right and unbind the framebuffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//bind to the default framebuffer

}

void FrameBuffer::Bind(GLuint unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Delete()
{
	glDeleteFramebuffers(1, &ID);
}

void FrameBuffer::attachTex2D(Texture& texture) {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.ID, 0);
}

void FrameBuffer::attachCubeFace(GLuint cubemapID, int iFace) {
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + iFace, cubemapID, 0);

	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Status error: %08x\n", status);
}
