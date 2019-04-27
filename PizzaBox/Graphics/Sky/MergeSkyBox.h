#ifndef MERGE_SKY_BOX_H
#define MERGE_SKY_BOX_H

#include "Sky.h"
#include "SkyBoxResource.h"
 
namespace PizzaBox{
	class MergeSkyBox : public Sky{
	public:
		explicit MergeSkyBox(const std::string& skyboxFile_, const std::string& skyboxFile2_, float size = 1000.0f, float rotateSpeed_ = 0.0f, const std::string& shaderName_ = "MergeSkyBoxShader");
		virtual ~MergeSkyBox() override;

		bool Initialize() override;
		void Destroy() override;

		void Render(Camera* camera) override; 
		GLuint GetSkyboxTexture() override;

	private: 
		std::string fileName, fileName2;
		SkyBoxResource* file, *file2;
		float time;

		Uniform* blendFactorID;
		Uniform* skybox1ID;
		Uniform* skybox2ID;

		void BindTexture();
	};
}
#endif //!MERGE_SKY_BOX_H