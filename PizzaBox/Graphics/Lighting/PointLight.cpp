#include "PointLight.h"

#include "../Shader.h"
#include "../RenderEngine.h"
#include "../../Math/Math.h"

using namespace PizzaBox;

PointLight::PointLight(float intensity_, const Color& lightColor_, float range_) : LightSource(intensity_, lightColor_), linear(0.09f), quadratic(0.032f){
	ambient = Color(0.05f, 0.05f, 0.05f);
	diffuse = Color(0.8f, 0.8f, 0.8f);
	specular = Color(1.0f, 1.0f, 1.0f);
	
	SetRange(range_);
}

PointLight::~PointLight(){
}

bool PointLight::Initialize(GameObject* go_){
	//Makes sure the GameObject is a valid pointer
	_ASSERT(go_ != nullptr);
	gameObject = go_;

	RenderEngine::RegisterPointLight(this);
	return true;
}

void PointLight::Destroy(){
	RenderEngine::UnregisterPointLight(this);
	gameObject = nullptr;
}

void PointLight::SetRange(float range_){
	range_ *= 10.0f;

	linear = (constant / Math::Sqrt(range_)) / range_;
	quadratic = ((constant - linear) / Math::Sqrt(range_)) / range_;
}