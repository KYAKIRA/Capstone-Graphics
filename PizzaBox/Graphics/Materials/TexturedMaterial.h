#ifndef TEXTURED_MATERIAL_H
#define TEXTURED_MATERIAL_H

#include "MeshMaterial.h"
#include "Graphics/Texture.h"

namespace PizzaBox{
	class TexturedMaterial : public MeshMaterial{
	public:
		TexturedMaterial(const std::string& diffMap_, bool animated_ = false, const std::string& specMap_ = "", const std::string& normalMap_= "", float shiny_ = 32.0f, float textureScale_ = 1.0f);
		virtual ~TexturedMaterial() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;

		virtual void Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights_) const;

		inline Texture* GetDiffuseMap(){ return diffuseMap; }
		inline Texture* GetSpecularMap(){ return specularMap; }
		inline Texture* GetNormalMap() { return normalMap; }
		inline float GetShininess(){ return shininess; }
		inline float GetTextureScale(){ return textureScale; }

		inline void SetShininess(float s_){ shininess = s_; }
		inline void SetTextureScale(float scale_){ textureScale = scale_; }

	private:
		std::string diffuseMapName;
		std::string specularMapName;
		std::string normalMapName;
		Texture* diffuseMap;
		Texture* specularMap;
		Texture* normalMap;
		float shininess;
		float textureScale;
	};
}

#endif // !TEXTURED_MATERIAL_H