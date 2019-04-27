#include "ReflectiveMaterial.h"

#include "Core/SceneManager.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Effects/Shadows.h"
#include "Resource/ResourceManager.h"

using namespace PizzaBox;

ReflectiveMaterial::ReflectiveMaterial(bool isReflective_, float refractionIndex_) : MeshMaterial("ReflectiveShader"), isReflective(isReflective_), refractionIndex(refractionIndex_){
}

ReflectiveMaterial::~ReflectiveMaterial(){
}

bool ReflectiveMaterial::Initialize(){
	shader = ResourceManager::LoadResource<Shader>(shaderName);
	if(shader == nullptr){
		Debug::LogError(shaderName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	SetupUniforms();

	return true;
}

void ReflectiveMaterial::Destroy(){
	CleanupUniforms();

	if(shader != nullptr){
		ResourceManager::UnloadResource(shaderName);
		shader = nullptr;
	}
}

void ReflectiveMaterial::Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights) const{
	shader->Use();

	shader->BindMatrix4("projectionMatrix", camera_->GetProjectionMatrix());
	shader->BindMatrix4("viewMatrix", camera_->GetViewMatrix());
	shader->BindMatrix4("modelMatrix", model_);
	shader->BindVector3("viewPos", camera_->GetGameObject()->GlobalPosition());

	shader->BindInt("isReflective", isReflective);
	shader->BindFloat("refractionIndex", refractionIndex);
	shader->BindCubeMap(GL_TEXTURE0, SceneManager::CurrentScene()->GetSky()->GetSkyboxTexture());
	
	shader->BindColor("fogColor", RenderEngine::GetFogColor());
	shader->BindFloat("density", RenderEngine::GetFogDensity());
	shader->BindFloat("gradient", RenderEngine::GetFogGradient());
}