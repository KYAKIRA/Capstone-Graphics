#include "ShadowFBO.h"

using namespace PizzaBox;

ShadowFBO::ShadowFBO(const ScreenCoordinate& screenSize_) : BaseFBO(screenSize_), depthMap(0){
}

ShadowFBO::~ShadowFBO(){
}

bool ShadowFBO::Initialize(){
	//Create framebuffer
	glGenFramebuffers(1, &frameBuffer);
	Bind();

	//Creates depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenSize.x, screenSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	Color borderColor = Color::White;
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//Attach to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE); 

	Unbind();
	return CheckFBOStatus();
}

void ShadowFBO::Destroy(){
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &depthMap);
}