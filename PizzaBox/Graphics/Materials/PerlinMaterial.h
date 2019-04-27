#ifndef PERLIN_MATERIAL_H
#define PERLIN_MATERIAL_H

#include "MeshMaterial.h"
#include "Graphics/Texture.h"

namespace PizzaBox{
	class PerlinMaterial : public MeshMaterial{
	public:
		PerlinMaterial(const std::string& diffMap_, const std::string& specMap_ = "", const std::string& normalMap_ = "", float shiny_ = 32.0f, float speedMult_ = 5.0f, float offsetMult_ = 25.0f, bool textureisMoving_ = true);
		virtual ~PerlinMaterial() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;

		void Update() override;
		virtual void Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights_) const;

		GLuint CreateNoise3D();
		GLuint Init3DNoiseTexture(int texSize, GLubyte* texPtr);

		float Noise1(float arg);
		float Noise2(const Vector2& vec);
		float Noise3(const Vector3& vec);
		float PerlinNoise2D(const Vector2& v, float alpha, float beta, int n);
		float PerlinNoise3D(const Vector3& v, float alpha, float beta, int n);
		void InitNoise();
		void Make3DNoiseTexture();
		void SetNoiseFrequency(int frequency);

		inline void SetTextureScale(float scale_){ textureScale = scale_; }

	private:
		bool textureIsMoving;
		float shininess;
		float time;
		float speedMult;
		float offsetMult;
		std::string diffuseMapName;
		std::string specularMapName;
		std::string normalMapName;
		Texture* diffuseMap;
		Texture* specularMap;
		Texture* normalMap;
		Vector3 noisePosition;
		float textureScale;
		GLuint noiseTexture;

		static bool start;
		static int B;
		static int BM;

		static int p[514];
		static float g1[514];
		static Vector2 g2[514];
		static Vector3 g3[514];

		static constexpr int noiseTextureSize = 128;
		static GLubyte* Noise3DTexPtr;

		static constexpr float SCurve(float t_);
		static constexpr float Lerp(float t_, float a_, float b_);
		static const void Setup(float& t_, float vec, int& b0, int& b1, float& r0, float& r1);
	};
}

#endif //!PERLIN_MATERIAL_H