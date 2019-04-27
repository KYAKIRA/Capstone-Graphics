#ifndef MESH_RENDER_H
#define MESH_RENDER_H

#include "Model.h"
#include "Graphics/Materials/MeshMaterial.h"
#include "Object/Component.h"

namespace PizzaBox{
	class MeshRender : public Component{
	public:
		MeshRender(const std::string& modelName_);
		MeshRender(const std::string& modelName_, MeshMaterial* material_);
		MeshRender(const std::string& modelName_, const std::string& textureName_);
		MeshRender(const std::string& modelName_, const Color& color_);
		MeshRender(const std::string& modelName_, const std::vector<MeshMaterial*>& materials_);
		~MeshRender();

		bool Initialize(GameObject* go_) override;
		void Destroy() override;

		void Render(Camera* camera_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights_);
		void SetMaterial(MeshMaterial* material_, size_t index_ = 0);
		inline Model* GetModel() { return model; }
		inline bool CastsShadows(){ return castsShadows; }

		inline void SetCastsShadows(bool casts_){ castsShadows = casts_; }

	private:
		std::string modelName;
		Model* model;
		std::vector<MeshMaterial*> materials;
		bool castsShadows;
	};
}

#endif //!MESH_RENDER_H