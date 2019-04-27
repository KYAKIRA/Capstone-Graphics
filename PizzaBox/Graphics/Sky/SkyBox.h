#ifndef SKY_BOX_H
#define SKY_BOX_H

#include "Sky.h"
#include "SkyBoxResource.h" 

namespace PizzaBox{
	class SkyBox : public Sky{
	public:
		explicit SkyBox(const std::string& skyboxFile_, float size_ = 1000.0f, float rotateSpeed_ = 0.0f, const std::string& shaderName_ = "SkyBoxShader");
		virtual ~SkyBox() override;

		bool Initialize() override;
		void Destroy() override;

		void Render(Camera* camera_) override; 
		GLuint GetSkyboxTexture() override; 

	private: 
		std::string fileName;
		SkyBoxResource* file; 
	};
}

#endif //!SKY_BOX_H
