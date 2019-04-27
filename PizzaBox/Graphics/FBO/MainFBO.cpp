#include "MainFBO.h"

using namespace PizzaBox;

MainFBO::MainFBO(const ScreenCoordinate& screenSize_) : BaseFBO(screenSize_), colorTexture(0){
}

MainFBO::~MainFBO(){
}

bool MainFBO::Initialize(){
	//Create framebuffer
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

	Unbind();
	return CheckFBOStatus();
}

void MainFBO::Destroy(){
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &colorTexture);
}