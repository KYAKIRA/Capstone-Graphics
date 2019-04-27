#include "ParticleTexture.h"

#include <rttr/registration.h>

#include "Resource/ResourceManager.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<ParticleTexture>("ParticleTexture")
		.constructor<const std::string&, int>()
		.method("GetTexture", &ParticleTexture::GetTexture)
		.method("GetNumOfTexture", &ParticleTexture::GetNumOfTexture);
}
#pragma warning( pop )

ParticleTexture::ParticleTexture(const std::string& filePath_, int numOfTexture_) : textureName(filePath_), numOfTexture(numOfTexture_){
	texture = ResourceManager::LoadResource<Texture>(textureName);
	if(texture == nullptr){
		Debug::LogError(filePath_ + " could not be loaded!", __FILE__, __LINE__);
		throw std::exception("Texture initialization failure!");
	}
}

ParticleTexture::~ParticleTexture(){
	ResourceManager::UnloadResource(textureName);
}