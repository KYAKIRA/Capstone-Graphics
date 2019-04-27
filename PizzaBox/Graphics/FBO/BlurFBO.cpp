#include "BlurFBO.h"

using namespace PizzaBox;

BlurFBO::BlurFBO(const ScreenCoordinate& screenSize_) : BaseFBO(screenSize_), colorTexture(0), depthBuffer(0){
}

BlurFBO::~BlurFBO(){
}

bool BlurFBO::Initialize(){
	//Create the frame buffer
	glGenFramebuffers(1, &frameBuffer);
	Bind();

	//Create color texture
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenSize.x, screenSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, colorTexture, 0);

	//Create depth buffer
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screenSize.x, screenSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	Unbind();
	return CheckFBOStatus();
}

void BlurFBO::Destroy(){
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteRenderbuffers(1, &colorTexture);
	glDeleteRenderbuffers(1, &depthBuffer);
}