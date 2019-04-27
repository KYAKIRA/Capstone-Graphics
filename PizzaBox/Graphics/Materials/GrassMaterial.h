#ifndef GRASS_MATERIAL_H
#define GRASS_MATERIAL_H

#include "MeshMaterial.h"
#include "Graphics/Texture.h"

namespace PizzaBox{
	class GrassMaterial : public MeshMaterial{
	public:
		GrassMaterial(const std::string& diffMap_, const std::string& specMap_ = "", const std::string& normalMap_ = "", float shiny_ = 32.0f, const Vector3& sway_ = Vector3(0.5f, 0.3f, 0.1f), float frequency_ = 0.025f);
		virtual ~GrassMaterial() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;

		virtual void Update() override;
		virtual void Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights_) const;

		Texture* GetDiffuseMap() const{ return diffuseMap; }
		Texture* GetSpecularMap() const{ return specularMap; }
		Texture* GetNormalMap() const{ return normalMap; }
		float GetShininess() const{ return shininess; }

		void SetShininess(float s_){ shininess = s_; }
		inline void SetTextureScale(float scale_){ textureScale = scale_; }

	private:
		std::string diffuseMapName;
		std::string specularMapName;
		std::string normalMapName;
		Texture* diffuseMap;
		Texture* specularMap;
		Texture* normalMap;
		Vector3 sway;
		float frequency;
		float shininess;
		float textureScale;
		float time;
	};
}

#endif //!GRASS_MATERIAL_H