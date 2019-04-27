#include "WaterMaterial.h"

#include "Graphics/RenderEngine.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Resource/ResourceManager.h"
#include "Tools/Random.h"

using namespace PizzaBox;

WaterMaterial::WaterMaterial(const std::string& diffMap_, const std::string& specMap_, const std::string& normalMap_, float shiny_, float textureScale_, float waterHeight_, int waveAmount_, float amplitude_, float waveLength_, float speed_) : MeshMaterial("WaterShader"), 
	diffuseMapName(diffMap_), specularMapName(specMap_), specularMap(nullptr), diffuseMap(nullptr), normalMapName(normalMap_), normalMap(nullptr), shininess(shiny_), textureScale(textureScale_), 
	waterHeight(waterHeight_), waveAmount(waveAmount_), time(0.0f), transparency(0.9f), flowDirection(Vector2()){

	_ASSERT(!diffuseMapName.empty());

	waveAmount = Math::Clamp(1, 4, waveAmount);

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

	//Temporarily store the initial values in the list
	amplitudeList.push_back(amplitude_);
	speedList.push_back(speed_);
	waveLengthList.push_back(waveLength_);
	directionList.push_back(Vector3(0.0f, 0.0f, 0.0f));

	ReceivesShadows(false);
}

WaterMaterial::~WaterMaterial(){
}

bool WaterMaterial::Initialize(){
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

	//Grab the initial values that we stored in the list and put them into temporary variables
	float amplitude = amplitudeList.front();
	float speed = speedList.front();
	float waveLength = waveLengthList.front();
	Vector3 direction = directionList.front();
	
	//Empty out our lists and reserve the relevant size
	amplitudeList.clear();
	amplitudeList.reserve(4);
	speedList.clear();
	speedList.reserve(4);
	waveLengthList.clear();
	waveLengthList.reserve(4);
	directionList.clear();
	directionList.reserve(4);

	for(unsigned int i = 0; i < 4; i++){
		waveLength *= Math::PI() / (i + 1);
		//speed of the waves
		speed += 2 * i;
		constexpr float min = -Math::PI() / 3.0f;
		constexpr float max = Math::PI() / 3.0f;

		float angle = Random::Range(min, max);
		
		direction = Vector3(sin(angle) * i, cos(angle) * i, cos(angle)  * i);
		amplitude /= (i + 1);
		waveLength *= Math::PI() / (i + 1);
		speed += 2 * i;

		amplitudeList.push_back(amplitude);
		waveLengthList.push_back(waveLength);
		speedList.push_back(speed);
		directionList.push_back(direction);
	}
	
	SetWaveParamaters();
	
	return true;
}

void WaterMaterial::Destroy(){
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

void WaterMaterial::Update(){
	time += Time::DeltaTime();
}

void WaterMaterial::Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights, const std::vector<PointLight*>& pointLights, const std::vector<SpotLight*>& spotLights) const{
	shader->Use();

	shader->BindMatrix4("projectionMatrix", camera_->GetProjectionMatrix());
	shader->BindMatrix4("viewMatrix", camera_->GetViewMatrix());
	shader->BindMatrix4("modelMatrix", model_);
	shader->BindMatrix3("normalMatrix", Matrix3(model_));
	shader->BindVector3("viewPos", camera_->GetGameObject()->GlobalPosition());
	shader->BindFloat("time", time); 
	shader->BindVector2("flowDirection", flowDirection * (time / 1000.0f));
	for(unsigned int i = 0; i < amplitudeList.size(); i++){
		shader->BindFloat("amplitude[" + std::to_string(i) + "]", amplitudeList[i]);
		shader->BindFloat("waveLength[" + std::to_string(i) + "]", waveLengthList[i]);
		shader->BindFloat("speed[" + std::to_string(i) + "]", speedList[i]);
		shader->BindVector3("direction[" + std::to_string(i) + "]", directionList[i]);
	}

	shader->BindFloat("waterHeight", waterHeight);
	shader->BindInt("waveAmount", waveAmount);
	shader->BindFloat("transparency", transparency);

	shader->BindColor("baseAmbient", RenderEngine::baseAmbient); //Base Lighting on all Object so they appear 

	shader->BindInt("numDirLights", static_cast<int>(dirLights.size()));
	for(unsigned int i = 0; i < dirLights.size(); i++){
		shader->BindDirectionalLight(i, dirLights[i]);
	}

	shader->BindInt("numPointLights", static_cast<int>(pointLights.size()));
	for(unsigned int i = 0; i < pointLights.size(); i++){
		shader->BindPointLight(i, pointLights[i]);
	}

	shader->BindInt("numSpotLights", static_cast<int>(spotLights.size()));
	for(unsigned int i = 0; i < spotLights.size(); i++){
		shader->BindSpotLight(i, spotLights[i]);
	}

	shader->BindTexture("material.diffuseMap", diffuseMap->TextureID());
	shader->BindTexture("material.specularMap", specularMap->TextureID());
	shader->BindFloat("material.shininess", shininess);
	shader->BindInt("material.receivesShadows", receivesShadows);
	shader->BindFloat("material.textureScale", textureScale);

	shader->BindColor("fogColor", RenderEngine::GetFogColor());
	shader->BindFloat("fogDensity", RenderEngine::GetWaterFogDensity());
	shader->BindFloat("fogGradient", RenderEngine::GetWaterFogGradient());
}

void WaterMaterial::SetWaveParamaters(const Vector4& waveLengthList_, const Vector4& amplitudeList_, const Vector4& speedList_, const Vector3& waveDirection1_, const Vector3& waveDirection2_, const Vector3& waveDirection3_, const Vector3& waveDirection4_){
	waveLengthList[0] = waveLengthList_.x;
	speedList[0] = speedList_.x;
	amplitudeList[0] = amplitudeList_.x;
	directionList[0] = waveDirection1_;

	if(waveAmount == 2){
		waveLengthList[1] = waveLengthList_.y;
		speedList[1] = speedList_.y;
		amplitudeList[1] = amplitudeList_.y;
		directionList[1] = waveDirection2_;

		if(waveAmount == 3){
			waveLengthList[2] = waveLengthList_.z;
			speedList[2] = speedList_.z;
			amplitudeList[2] = amplitudeList_.z;
			directionList[2] = waveDirection3_;
		}

		if(waveAmount == 4){
			waveLengthList[3] = waveLengthList_.w;
			speedList[3] = speedList_.w;
			amplitudeList[3] = amplitudeList_.w;
			directionList[3] = waveDirection4_;
		}
	}
}