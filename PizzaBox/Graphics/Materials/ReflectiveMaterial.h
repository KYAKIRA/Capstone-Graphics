#ifndef REFLECTIVE_MATERIAL_H
#define REFLECTIVE_MATERIAL_H

#include "MeshMaterial.h"

namespace PizzaBox{
	class ReflectiveMaterial : public MeshMaterial{
	public:
		ReflectiveMaterial(bool isReflective_ = true, float refractionIndex_ = water);
		virtual ~ReflectiveMaterial() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;
		virtual void Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights) const override;

		//---------REFRACTION INDEX---------
		static constexpr float air = 1.00f;
		static constexpr float water = 1.33f;
		static constexpr float ice = 1.309f;
		static constexpr float glass = 1.52f;
		static constexpr float diamond = 2.42f;
		
	protected:
		float refractionIndex;
		bool isReflective;
	};
}

#endif //!REFLECTIVE_MATERIAL_H