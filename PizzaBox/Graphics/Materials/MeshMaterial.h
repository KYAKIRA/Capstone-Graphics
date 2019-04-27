#ifndef MESH_MATERIAL_H
#define MESH_MATERIAL_H

#include "BaseMaterial.h"
#include "Graphics/Camera.h"
#include "Graphics/Lighting/DirectionalLight.h"
#include "Graphics/Lighting/PointLight.h"
#include "Graphics/Lighting/SpotLight.h"

namespace PizzaBox{
	class MeshMaterial : public BaseMaterial{
	public:
		MeshMaterial(const std::string& shader_) : BaseMaterial(shader_), receivesShadows(true){}
		virtual ~MeshMaterial() override{}

		virtual bool Initialize() override = 0;
		virtual void Destroy() override = 0;

		virtual void Update(){}
		virtual void Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights_) const = 0;

		inline void ReceivesShadows(bool receivesShadows_){ receivesShadows = receivesShadows_; }

	protected:
		bool receivesShadows;
	};
}

#endif //!MESH_MATERIAL_H