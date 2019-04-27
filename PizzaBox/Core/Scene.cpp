#include "Scene.h"

#include <rttr/registration.h>

#include "GameManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Scene>("Scene")
		.method("Initialize", &Scene::Initialize)
		.method("Destroy", &Scene::Destroy)
		.method("Update", &Scene::Update)
		//TODO - Template Functions
		.method("DestroyObject", &Scene::DestroyObject)
		.method("AttachObject", &Scene::AttachObject)
		.method("DetachObject", &Scene::DetachObject)
		.method("FindWithTag", &Scene::FindWithTag)
		.method("FindObjectsWithTag", &Scene::FindObjectsWithTag)
		.method("GetSky", &Scene::GetSky)
		.method("SetSky", &Scene::SetSky);
}
#pragma warning( pop )

Scene::Scene() : gameObjectList(std::vector<GameObject*>()), sky(nullptr), createQueue(std::queue<GameObject*>()), destroyQueue(std::queue<GameObject*>()), skyQueue(std::queue<Sky*>()){
}

Scene::~Scene(){
	//This shouldn't be necessary but in Debug mode we'll do it anyway just to be safe and avoid memory leaks
#ifdef _DEBUG
	if(!gameObjectList.empty() || sky != nullptr || !createQueue.empty() || !destroyQueue.empty() || !skyQueue.empty()){
		Debug::LogWarning("Memory leak detected in Scene!", __FILE__, __LINE__);
		Debug::LogWarning("Make sure you call Scene::Destroy, or don't override Destroy if it doesn't do anything.");
		Scene::Destroy();
	}
#endif //_DEBUG
}

void Scene::Destroy(){
	for(GameObject* go : gameObjectList){
		if(go != nullptr){
			go->Destroy();
			delete go;
			go = nullptr;
		}
	}

	gameObjectList.clear();
	gameObjectList.shrink_to_fit();

	if(sky != nullptr){
		sky->Destroy();
		delete sky;
		sky = nullptr;
	}

	while(!createQueue.empty()){
		if(createQueue.front() != nullptr){
			createQueue.front()->Destroy();
			delete createQueue.front();
			createQueue.pop();
		}
	}

	while(!destroyQueue.empty()){
		//Objects in the destroyQueue will have already been deleted in the gameObjectList
		//Attempting to delete them again here would cause memory corruption
		destroyQueue.pop();
	}

	while(!skyQueue.empty()){
		skyQueue.front()->Destroy();
		delete skyQueue.front();
		skyQueue.pop();
	}
}

bool Scene::Update(){
	if(MaintainQueues() == false){
		Debug::LogError("Scene Queue Maintenance Error!", __FILE__, __LINE__);
		return false;
	}

	for(GameObject* go : gameObjectList){
		if(go->Update() == false){
			Debug::LogError("GameObject could not be updated!", __FILE__, __LINE__);
			return false;
		}
	}

	if(MaintainQueues() == false){
		Debug::LogError("Scene Queue Maintenance Error!", __FILE__, __LINE__);
		return false;
	}

	return true;
}

void Scene::DestroyObject(GameObject* go_){
	//Must be a valid GameObject pointer
	_ASSERT(go_ != nullptr);
	destroyQueue.push(go_);
}

void Scene::AttachObject(GameObject* go_){
	attachQueue.push(go_);
}

void Scene::DetachObject(GameObject* go_){
	detachQueue.push(go_);
}

GameObject* Scene::FindWithTag(const std::string& tag_){
	for(GameObject* go : gameObjectList){
		if(go->HasTag(tag_)){
			return go;
		}
	}

	return nullptr;
}

std::vector<GameObject*> Scene::FindObjectsWithTag(const std::string& tag_){
	std::vector<GameObject*> objectsWithTag;

	for(GameObject* go : gameObjectList){
		if(go->HasTag(tag_)){
			objectsWithTag.push_back(go);
		}
	}

	return objectsWithTag;
}

Sky* Scene::GetSky(){
	return sky;
}

void Scene::SetSky(Sky* sky_){
	_ASSERT(sky_ != nullptr);
	skyQueue.push(sky_);
}

bool Scene::MaintainQueues(){
	//Create any objects that are set to be created
	while(!createQueue.empty()){
		_ASSERT(createQueue.front() != nullptr);
		GameObject* go = createQueue.front();
		createQueue.pop();

		if(go->Initialize() == false){
			Debug::LogError("Could not create GameObject!", __FILE__, __LINE__);
			GameManager::Stop();
			return false;
		}

		gameObjectList.push_back(go);
	}

	//Destroy any objects that are set to be destroyed
	while(!destroyQueue.empty()){
		_ASSERT(destroyQueue.front() != nullptr);
		GameObject* go = destroyQueue.front();
		destroyQueue.pop();

		go->Destroy();
		delete go;
		gameObjectList.erase(std::remove(gameObjectList.begin(), gameObjectList.end(), go), gameObjectList.end());
	}

	while(!skyQueue.empty()){
		//Delete the current sky
		if(sky != nullptr){
			sky->Destroy();
			delete sky;
			sky = nullptr;
		}

		sky = skyQueue.front();
		skyQueue.pop();
		if(sky->Initialize() == false){
			return false;
		}
	}

	while(!attachQueue.empty()){
		_ASSERT(attachQueue.front() != nullptr);
		GameObject* go = attachQueue.front();
		attachQueue.pop();

		gameObjectList.push_back(go);
	}

	while(!detachQueue.empty()){
		_ASSERT(detachQueue.front() != nullptr);
		GameObject* go = detachQueue.front();
		detachQueue.pop();

		gameObjectList.erase(std::remove(gameObjectList.begin(), gameObjectList.end(), go), gameObjectList.end());
	}

	return true;
}