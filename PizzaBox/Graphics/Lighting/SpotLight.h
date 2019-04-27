#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H
 
#include "LightSource.h"

namespace PizzaBox{
	class SpotLight : public LightSource{
	public:
		SpotLight(float intensity_ = 1.0f, const Color& lightColor_ = Color::White, float range_ = 1.0f);
		~SpotLight();

		bool Initialize(GameObject* go_) override;
		void Destroy() override;

		inline float GetCutOff() const{ return cutOff; }
		inline float GetOuterCutOff() const{ return outerCutOff; }
		inline float GetConstant() const{ return constant; }
		inline float GetLinear() const{ return linear; }
		inline float GetQuadratic() const{ return quadratic; }

		inline void SetCutOff(float cutOff_){ cutOff = cutOff_; }
		inline void SetOuterCutOff(float outerCutOff_){ outerCutOff = outerCutOff_; }
		inline void SetLinear(float linear_){ linear = linear_; }
		inline void SetQuadratic(float quadratic_){ quadratic = quadratic_; }
		void SetRange(float range_);

	private:
		float cutOff;
		float outerCutOff;

		static constexpr float constant = 1.0f;
		float linear;
		float quadratic; 
	};
}

#endif //!SPOT_LIGHT_H