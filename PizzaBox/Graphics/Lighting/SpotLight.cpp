#include "SpotLight.h"

#include "Graphics/Shader.h"
#include "Graphics/RenderEngine.h"
#include "Math/Math.h"

using namespace PizzaBox;

SpotLight::SpotLight(float intensity_, const Color& lightColor_, float range_) : LightSource(intensity_, lightColor_), cutOff(12.5f), outerCutOff(0.91f), linear(0.09f), quadratic(0.032f){
	ambient = Color(0.1f, 0.1f, 0.1f);
	diffuse = Color(1.0f, 1.0f, 1.0f);
	specular = Color(1.0f, 1.0f, 1.0f);

	SetRange(range_);
}

SpotLight::~SpotLight(){
}

bool SpotLight::Initialize(GameObject* go_){
	//Makes sure the GameObject is a valid pointer
	_ASSERT(go_ != nullptr);
	gameObject = go_;

	RenderEngine::RegisterSpotLight(this);
	return true;
}

void SpotLight::Destroy(){
	RenderEngine::UnregisterSpotLight(this);
	gameObject = nullptr;
}

void SpotLight::SetRange(float range_){
	range_ *= 10.0f;

	linear = (constant / Math::Sqrt(range_)) / range_;
	quadratic = ((constant - linear) / Math::Sqrt(range_)) / range_;
}