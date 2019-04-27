#include "ResourceManager.h"

#include <iostream>

#include "ResourceParser.h"

using namespace PizzaBox;

//Initialize Static Variables Here
std::map<const std::string, ResourceProfile*> ResourceManager::resources = std::map<const std::string, ResourceProfile*>();

bool ResourceManager::Initialize(){
	if(ResourceParser::ParseEngineResources() == false){
		Debug::LogError("Could not parse engine resources!", __FILE__, __LINE__);
		return false;
	}

	if(ResourceParser::ParseGameResources() == false){
		Debug::LogError("Could not parse game resources!", __FILE__, __LINE__);
		return false;
	} 

	return true;
}

void ResourceManager::Destroy(){
	for(auto i = resources.begin(); i != resources.end(); i++){
		delete i->second;
		i->second = nullptr;
	}

	resources.clear();
}

void ResourceManager::AddResource(const std::string& resourceName_, Resource* resource_){
	//We'll use a lower case version of the name so that creating resources is always case-insensitive
	std::string name;
	std::transform(resourceName_.begin(), resourceName_.end(), std::back_inserter(name), tolower);

	if(resources.find(name) != resources.end()){
		Debug::LogWarning("Resource with name [" + resourceName_ + "] already exists in ResourceManager!");
		return;
	}

	ResourceProfile* r = new ResourceProfile(resource_, false);
	resources.insert(std::make_pair(name, r));
}

void ResourceManager::AddPermanentResource(const std::string& resourceName_, Resource* resource_){
	//We'll use a lower case version of the name so that creating resources is always case-insensitive
	std::string name;
	std::transform(resourceName_.begin(), resourceName_.end(), std::back_inserter(name), tolower);

	//This assertion will trigger if a resource with this name is already in the map
	_ASSERT(resources.find(name) == resources.end());

	if(resources.find(name) == resources.end()){
		//If there isn't a resource with this name in the map then create one
		ResourceProfile* r = new ResourceProfile(resource_, true);
		resources.insert(std::make_pair(name, r));
	}
}

void ResourceManager::UnloadResource(const std::string& resourceName_){
	//We'll use a lower case version of the name so that unloading resources is always case-insensitive
	std::string name;
	std::transform(resourceName_.begin(), resourceName_.end(), std::back_inserter(name), tolower);

	//This assertion will trigger if the resource name is invalid
	_ASSERT(resources.find(name) != resources.end());
	
	if(resources[name]->loadCount > 0){
		resources[name]->loadCount--;
	}

	if(resources[name]->loadCount == 0 && resources[name]->isPermanent == false){
		//If this resource isn't being used anywhere and isn't a permanent resource then we unload it
		resources[name]->resourcePtr->Unload();
	}
}

void ResourceManager::LoadPermanentResources(){
	for(const auto& r : resources){
		if(!r.second->isPermanent){
			continue;
		}

		LoadResource<Resource>(r.first);
	}
}

void ResourceManager::UnloadPermanentResources(){
	for(const auto& r : resources){
		if(!r.second->isPermanent){
			continue;
		}

		while(r.second->loadCount > 0){
			UnloadResource(r.first);
		}

		r.second->resourcePtr->Unload();
	}
}