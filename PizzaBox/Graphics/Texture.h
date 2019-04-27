#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include <glew.h>

#include "../Resource/Resource.h"

namespace PizzaBox{
	class Texture : public Resource{
	public:
		explicit Texture(const std::string filePath_);
		~Texture();

		bool Load();
		void Unload();

		GLuint TextureID();
	private:
		GLuint textureID;
	};
}

#endif //!TEXTURE_H