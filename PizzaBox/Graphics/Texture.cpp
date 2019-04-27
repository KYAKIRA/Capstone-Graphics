#include "Texture.h"

#include <SDL_Image.h>
#include <glew.h>

#include "Tools/Debug.h"

using namespace PizzaBox;

Texture::Texture(const std::string filePath) : Resource(filePath), textureID(0){
}

Texture::~Texture(){
}

bool Texture::Load(){
	//Generate the Texture in OpenGL and get the ID
	glGenTextures(1, &textureID);
	//Bind the generated texture

	glBindTexture(GL_TEXTURE_2D, textureID);

	glGenerateMipmap(GL_TEXTURE_2D);

	//Load in the texture from the source file
	SDL_Surface* textureSurface = IMG_Load(fileName.c_str());
	if(textureSurface == nullptr){
		Debug::LogError(SDL_GetError(), __FILE__, __LINE__);
		return false;
	}

	//Use alpha if the image supports it, otherwise don't
	int mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	int precision = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA8 : GL_RGB8;

	//Wrapping and filtering options
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	//Load the texture data from the SDL_Surface to the GPU memmory
	glTexImage2D(GL_TEXTURE_2D, 0, precision, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	//Release the memory
	SDL_FreeSurface(textureSurface);
	textureSurface = nullptr;
	
	return true;
}

void Texture::Unload(){
	glDeleteTextures(1, &textureID);
}

GLuint Texture::TextureID(){
	return textureID;
}