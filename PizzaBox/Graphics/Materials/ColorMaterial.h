#ifndef COLOR_MATERIAL_H
#define COLOR_MATERIAL_H

#include "MeshMaterial.h"
#include "Graphics/Color.h"

namespace PizzaBox{
	class ColorMaterial : public MeshMaterial{
	public:
		ColorMaterial(const Color& color_, bool animated_ = false, float shiny_ = 32.0f);
		virtual ~ColorMaterial() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;

		virtual void Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights_) const override;

	private:
		Color color;
		float shininess;
	};
}

#endif //!COLOR_MATERIAL_H