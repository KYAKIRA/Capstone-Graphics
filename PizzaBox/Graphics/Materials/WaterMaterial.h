#ifndef WATER_MATERIAL_H
#define WATER_MATERIAL_H

#include "MeshMaterial.h"
#include "Graphics/Texture.h"
#include "Math/Math.h"

namespace PizzaBox{
	class WaterMaterial : public MeshMaterial{
	public:
		WaterMaterial(const std::string& diffMap_, const std::string& specMap_ = "", const std::string& normalMap_ = "", float shiny_ = 32.0f, float textureScale_ = 1.0f, float waterHeight_ = 0.0f, int waveAmount_ = 4,  float amplitude_ = 1.0f,  float waveLength_ = 12.0f, float speed_ = 1.0f);
		virtual ~WaterMaterial() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;

		virtual void Update() override;
		virtual void Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights, const std::vector<PointLight*>& pointLights, const std::vector<SpotLight*>& spotLights) const;
	
		inline float GetTextureScale(){ return textureScale; }
		inline void SetTextureScale(float scale_){ textureScale = scale_; }

		inline float GetTransparency(){ return transparency; }
		inline void SetTransparency(const float transparency_) { transparency = Math::Clamp(0.0f, 1.0f, transparency_); }

		inline void SetFlowDirection(const Vector2& flow_){ flowDirection = flow_.Normalized(); }

		void SetWaveParamaters(const Vector4& waveLengthList_ = Vector4(12.0f, 12.0f, 12.0f, 12.0f), const Vector4& amplitudeList_ = Vector4(0.4f, 0.4f, 0.4f, 0.4f), const Vector4& speedList_ = Vector4(0.5225f, 0.555f, 0.5425f, 0.25550f), const Vector3& waveDirection1_ = Vector3(-5.0f, 1.51f, 0.0f) , const Vector3& waveDirection2_ = Vector3(12.0f, -4.58f, 0.0f) , const Vector3& waveDirection3_ = Vector3(-12.0f, 4.52f, 0.0f), const Vector3& waveDirection4_ = Vector3(-12.50f, 5.58f, 0.0f));
	
	private:
		float waterHeight;
		int waveAmount;
		float time;
		std::string diffuseMapName;
		std::string specularMapName;
		std::string normalMapName;
		Texture* diffuseMap;
		Texture* specularMap;
		Texture* normalMap;
		float shininess;
		float textureScale;
		float transparency;
		std::vector<float> amplitudeList;
		std::vector<float> speedList;
		std::vector<float> waveLengthList;
		std::vector<Vector3> directionList;
		Vector2 flowDirection;
	};
}

#endif //!WATER_MATERIAl_H