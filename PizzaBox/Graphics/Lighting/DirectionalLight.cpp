#include "DirectionalLight.h"
#include "../Shader.h"
#include "../RenderEngine.h"

using namespace PizzaBox;

DirectionalLight::DirectionalLight(float intensity_, Color lightColor_) : LightSource(intensity_, lightColor_){
	ambient = Color(0.05f, 0.05f, 0.05f);
	diffuse = Color(0.4f, 0.4f, 0.4f);
	specular = Color(0.5f, 0.5f, 0.5f);

}
 
DirectionalLight::~DirectionalLight(){
}

bool DirectionalLight::Initialize(GameObject* go_){
	//Makes sure the GameObject is a valid pointer
	_ASSERT(go_ != nullptr);
	gameObject = go_;
	 
	RenderEngine::RegisterDirectionalLight(this);
	return true;
}

void DirectionalLight::Destroy(){
	RenderEngine::UnregisterDirectionalLight(this);
	gameObject = nullptr;
}