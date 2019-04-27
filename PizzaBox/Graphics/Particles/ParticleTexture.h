#ifndef PARTICLE_TEXTURE_H
#define PARTICLE_TEXTURE_H

#include <string>

#include <glew.h>

#include "../Texture.h"

namespace PizzaBox{
	class ParticleTexture{
	public:
		ParticleTexture(const std::string& filePath_, int numOfTexture_);
		~ParticleTexture();

		inline GLuint GetTexture() const { return texture->TextureID(); }
		inline int GetNumOfTexture() const { return numOfTexture; }

	private:
		std::string textureName;
		Texture* texture;
		int numOfTexture;
	};
}

#endif //!PARTICLE_TEXTURE_H