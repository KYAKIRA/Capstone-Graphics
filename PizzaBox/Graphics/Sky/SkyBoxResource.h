#ifndef SKY_BOX_RESOURCE_H
#define SKY_BOX_RESOURCE_H

#include <glew.h>

#include "../../Resource/Resource.h"

namespace PizzaBox{
	class SkyBoxResource : public Resource{
	public:
		explicit SkyBoxResource(const std::string fileName_);
		~SkyBoxResource();

		bool Load() override;
		void Unload() override;

		GLuint TextureID() const{
			return textureID;
		}

	private:
		GLuint textureID;
	};
}

#endif //!SKY_BOX_RESOURCE_H