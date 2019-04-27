#ifndef BLUR_H
#define BLUR_H

#include "../Shader.h"
#include "../FBO/BlurFBO.h"

namespace PizzaBox{
	class Blur{
	public:
		enum class Mode{
			horizontal,
			vertical
		};

		Blur(std::string shaderName_);
		~Blur();

		bool Initialize();
		void Destroy();

		void Update(Mode mode_, GLuint texture_, int blurStrength_);
		GLuint GetTexture(); 
		void OnResize();
	private: 
		Shader* shader; 
		BlurFBO* fbo;	 
		std::string shaderName;
	};
}

#endif //!BLUR_H