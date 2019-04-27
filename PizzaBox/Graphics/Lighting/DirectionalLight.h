#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H
 
#include "LightSource.h" 

namespace PizzaBox{
	class DirectionalLight : public LightSource{
	public:
		DirectionalLight(float intensity_ = 1.0f, Color lightColor_ = Color::White);
		~DirectionalLight();

		bool Initialize(GameObject* go_) override;
		void Destroy() override;
	};
}

#endif //!DIRECTIONAL_LIGHT_H