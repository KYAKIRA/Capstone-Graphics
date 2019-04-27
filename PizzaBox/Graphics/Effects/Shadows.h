#ifndef SHADOWS_H
#define SHADOWS_H

#include <list>

#include "ShadowBox.h"
#include "Animation/AnimMeshRender.h"
#include "Graphics/Camera.h"
#include "Graphics/Shader.h"
#include "Graphics/FBO/ShadowFBO.h"
#include "Graphics/Lighting/DirectionalLight.h"
#include "Graphics/Lighting/SpotLight.h"
#include "Graphics/Models/MeshRender.h"

namespace PizzaBox{
	class Shadows{
	public:
		Shadows(const std::string& shaderName_, unsigned int resolution_ = 1024);
		~Shadows();

		bool Initialize();
		void Destroy();

		void Render(const std::vector<Camera*>& cams_, const std::vector<MeshRender*>& mrs_, const std::vector<AnimMeshRender*>& amrs_, const std::vector<DirectionalLight*>& dirs_, const std::vector<SpotLight*>& spots_);

	private:
		unsigned int shadowResolution;
		ShadowBox* box;

		std::list<ShadowFBO*> depthFBOs;

		Shader* animDepthShader;
		Shader* depthShader;
		std::string animDepthShaderName;
		std::string depthShaderName;

		void ReserveFBOs(size_t numFBOs_);
	};
} 

#endif //!SHADOWS_H