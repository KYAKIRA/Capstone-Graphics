#include "GrassMaterial.h"

#include "Graphics/RenderEngine.h"
#include "Core/Time.h"
#include "Resource/ResourceManager.h"
#include "Tools/Random.h"

using namespace PizzaBox;

GrassMaterial::GrassMaterial(const std::string& diffMap_, const std::string& specMap_, const std::string& normalMap_, float shiny_, const Vector3& sway_, float frequency_) : MeshMaterial("GrassShader"), diffuseMapName(diffMap_), diffuseMap(nullptr), specularMap(nullptr), normalMapName(normalMap_), normalMap(nullptr), shininess(shiny_), sway(sway_), frequency(frequency_), time(0.0f), textureScale(1.0f){
	_ASSERT(!diffuseMapName.empty());

	if(specMap_.empty()){
		specularMapName = diffuseMapName;
	}else{
		specularMapName = specMap_;
	}

	if(normalMap_.empty()){
		normalMapName = diffuseMapName;
	}else{
		normalMapName = normalMap_;
	}
}

GrassMaterial::~GrassMaterial(){
}

bool GrassMaterial::Initialize(){
	shader = ResourceManager::LoadResource<Shader>(shaderName);
	if(shader == nullptr){
		Debug::LogError(shaderName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	diffuseMap = ResourceManager::LoadResource<Texture>(diffuseMapName);
	if(diffuseMap == nullptr){
		Debug::LogError(diffuseMapName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	specularMap = ResourceManager::LoadResource<Texture>(specularMapName);
	if(specularMap == nullptr){
		Debug::LogError(specularMapName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	normalMap = ResourceManager::LoadResource<Texture>(normalMapName);
	if(normalMap == nullptr){
		Debug::LogError(normalMapName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	return true;
}

void GrassMaterial::Destroy(){
	if(diffuseMap != nullptr){
		ResourceManager::UnloadResource(diffuseMapName);
		diffuseMap = nullptr;
	}

	if(specularMap != nullptr){
		ResourceManager::UnloadResource(specularMapName);
		specularMap = nullptr;
	}

	if(normalMap != nullptr){
		ResourceManager::UnloadResource(normalMapName);
		normalMap = nullptr;
	}

	if(shader != nullptr){
		ResourceManager::UnloadResource(shaderName);
		shader = nullptr;
	}
}

void GrassMaterial::Update(){
	time += Time::DeltaTime();
}

void GrassMaterial::Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights_) const{
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

	shader->BindFloat("time", time);
	shader->BindVector3("sway", sway);
	shader->BindFloat("frequency", frequency);

	shader->BindTexture("material.diffuseMap", diffuseMap->TextureID());
	shader->BindTexture("material.specularMap", specularMap->TextureID());
	shader->BindFloat("material.shininess", shininess);
	shader->BindInt("material.receivesShadows", receivesShadows);
	shader->BindFloat("material.textureScale", textureScale);

	shader->BindColor("fogColor", RenderEngine::GetFogColor());
	shader->BindFloat("fogDensity", RenderEngine::GetFogDensity());
	shader->BindFloat("fogGradient", RenderEngine::GetFogGradient());
}