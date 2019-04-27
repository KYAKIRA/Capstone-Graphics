#include "ColorMaterial.h"

#include <glew.h>

#include "Graphics/RenderEngine.h"
#include "Graphics/Effects/Shadows.h"
#include "Resource/ResourceManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

ColorMaterial::ColorMaterial(const Color& color_, bool animated_, float shiny_) : MeshMaterial("ColorShader"), color(color_), shininess(shiny_){
	if(animated_){
		shaderName = "AnimColorShader";
	}
}

ColorMaterial::~ColorMaterial(){
}

bool ColorMaterial::Initialize(){
	shader = ResourceManager::LoadResource<Shader>(shaderName);
	if(shader == nullptr){
		Debug::LogError(shaderName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}
	
	return true;
}

void ColorMaterial::Destroy(){
	if(shader != nullptr){
		ResourceManager::UnloadResource(shaderName);
		shader = nullptr;
	}
}

void ColorMaterial::Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector <SpotLight*>& spotLights_) const{
	shader->Use();

	shader->BindMatrix4("projectionMatrix", camera_->GetProjectionMatrix());
	shader->BindMatrix4("viewMatrix", camera_->GetViewMatrix());
	shader->BindMatrix4("modelMatrix", model_);
	shader->BindMatrix3("normalMatrix", model_.ToMatrix3());
	shader->BindVector3("viewPos", camera_->GetGameObject()->GlobalPosition());

	shader->BindColor("baseAmbient", RenderEngine::baseAmbient);

	shader->BindInt("numDirLights", static_cast<GLint>(dirLights_.size()));
	for(unsigned int i = 0; i < dirLights_.size(); i++){
		shader->BindDirectionalLight(i, dirLights_[i]);
	}

	shader->BindInt("numPointLights", static_cast<GLint>(pointLights_.size()));
	for(unsigned int i = 0; i < pointLights_.size(); i++){
		shader->BindPointLight(i, pointLights_[i]);
	}

	shader->BindInt("numSpotLights", static_cast<GLint>(spotLights_.size()));
	for(unsigned int i = 0; i < spotLights_.size(); i++){
		shader->BindSpotLight(i, spotLights_[i]);
	}

	shader->BindFloat("material.shininess", shininess);
	shader->BindColor("material.color", color);
	shader->BindInt("material.receivesShadows", receivesShadows);

	shader->BindColor("fogColor", RenderEngine::GetFogColor());
	shader->BindFloat("fogDensity", RenderEngine::GetFogDensity());
	shader->BindFloat("fogGradient", RenderEngine::GetFogGradient());
}