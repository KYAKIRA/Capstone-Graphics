#include "SkyBoxResource.h"

#include <vector>

#include <SDL_image.h>

#include "../../Tools/Debug.h"

using namespace PizzaBox;

SkyBoxResource::SkyBoxResource(const std::string fileName_) : Resource(fileName_), textureID(0){
}

SkyBoxResource::~SkyBoxResource(){
} 

bool SkyBoxResource::Load(){
	std::vector<std::string> faces = {
		fileName + "/right.png",
		fileName + "/left.png",
		fileName + "/up.png",
		fileName + "/down.png",
		fileName + "/back.png",
		fileName + "/front.png"
	};

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int mode;
	int precision;
	int expectedBytes;
	for (GLuint i = 0; i < faces.size(); i++) {
		SDL_Surface* image = IMG_Load(faces[i].c_str());
		if(image == nullptr){
			Debug::LogError("Could not load image " + faces[i] + "!", __FILE__, __LINE__);
			return false;
		}

		//This is the first runthrough of the loop, we want to set the mode
		if(i == 0){
			//Use alpha if the image supports it, otherwise don't
			expectedBytes = image->format->BytesPerPixel;
			mode = (expectedBytes == 4) ? GL_RGBA : GL_RGB;
			precision = (expectedBytes == 4) ? GL_RGBA8 : GL_RGB8;
			
		}else{
			//On any other run of the loop we just want to compare
			if(image->format->BytesPerPixel != expectedBytes){
				Debug::LogError("Skybox images have inconsistent bit depths! Was expecting a bit depth of " +
					std::to_string(expectedBytes) + " on " + faces[i] + "!", __FILE__, __LINE__
				);
				return false;
			}
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, precision, image->w, image->h, 0, mode, GL_UNSIGNED_BYTE, image->pixels);
		SDL_FreeSurface(image);
	}

	//Texture filters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}

void SkyBoxResource::Unload(){
	glDeleteTextures(1, &textureID);
}