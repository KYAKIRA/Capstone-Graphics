#include "MultisampleFBO.h"

using namespace PizzaBox;

MultisampleFBO::MultisampleFBO(const ScreenCoordinate& screenSize_) : BaseFBO(screenSize_), colorTexture(0), depthBuffer(0){
}

MultisampleFBO::~MultisampleFBO(){
}

bool MultisampleFBO::Initialize(){
	//Create the frame buffer
	glGenFramebuffers(1, &frameBuffer);
	Bind();

	//Create the multisampled color texture
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, screenSize.x, screenSize.y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorTexture, 0);

	//Create the multisampled depth buffer attachment
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, screenSize.x, screenSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	Unbind();
	return CheckFBOStatus();
}

void MultisampleFBO::Destroy(){
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteRenderbuffers(1, &colorTexture);
	glDeleteRenderbuffers(1, &depthBuffer);
}