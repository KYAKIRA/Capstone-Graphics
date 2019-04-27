#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "LightSource.h"

namespace PizzaBox{
	class PointLight : public LightSource{
	public:
		PointLight(float intensity_ = 1.0f, const Color& lightColor_ = Color::White, float range_ = 10.0f);
		~PointLight();

		bool Initialize(GameObject* go_) override;
		void Destroy() override;

		inline float GetConstant() const{ return constant; }
		inline float GetLinear() const{ return linear; }
		inline float GetQuadratic() const{ return quadratic; }

		inline void SetLinear(float linear_){ linear = linear_; }
		inline void SetQuadratic(float quadratic_){ quadratic = quadratic_; }
		void SetRange(float range_);
	
	private:
		static constexpr float constant = 1.0f;
		float linear;
		float quadratic;
	};
}

#endif //!POINT_LIGHT_H