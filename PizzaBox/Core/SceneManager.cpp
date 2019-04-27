#include "SceneManager.h"

#include <rttr/registration.h>

#include "Core/GameManager.h"
#include "Graphics/UI/UIManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

std::vector<Scene*> SceneManager::sceneList = std::vector<Scene*>();
int SceneManager::currentSceneIndex = -1;
std::queue<unsigned int> SceneManager::sceneLoadQueue = std::queue<unsigned int>();

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<SceneManager>("SceneManager")
		.method("Initialize", &SceneManager::Initialize)
		.method("Destroy", &SceneManager::Destroy)
		.method("Update", &SceneManager::Update)
		.method("AddScene", &SceneManager::AddScene)
		.method("LoadInitialScene", &SceneManager::LoadInitialScene)
		.method("LoadScene", &SceneManager::LoadScene)
		.method("LoadNextScene", &SceneManager::LoadNextScene)
		.method("ReloadCurrentScene", &SceneManager::ReloadCurrentScene)
		.method("CurrentScene", &SceneManager::CurrentScene)
		.method("CurrentSceneIndex", &SceneManager::CurrentSceneIndex)
		.method("NumScenes", &SceneManager::NumScenes)
		.method("TestLoadingAllScenes", &SceneManager::TestLoadingAllScenes);
}
#pragma warning( pop )

bool SceneManager::Initialize(){
	//These probably aren't necessary but it's best to play it safe
	currentSceneIndex = -1;
	sceneList.clear();
	sceneList.shrink_to_fit();

	while(!sceneLoadQueue.empty()){
		sceneLoadQueue.pop();
	}

	return true;
}

void SceneManager::Destroy(){
	for(Scene* scene : sceneList){
		if(scene != nullptr){
			scene->Destroy();
			delete scene;
			scene = nullptr;
		}
	}

	while(!sceneLoadQueue.empty()){
		sceneLoadQueue.pop();
	}

	sceneList.clear();
	sceneList.shrink_to_fit();
	currentSceneIndex = -1;
}

bool SceneManager::Update(){
	_ASSERT(currentSceneIndex != -1);
	_ASSERT(!sceneList.empty());
	_ASSERT(CurrentScene() != nullptr);

	if(!sceneLoadQueue.empty()){
		if(ChangeToNewScene() == false){
			Debug::LogError("Could not load new scene!", __FILE__, __LINE__);
			return false;
		}

		while(!sceneLoadQueue.empty()){
			sceneLoadQueue.pop();
		}
	}

	if(CurrentScene()->Update() == false){
		Debug::LogError("Could not update current scene!", __FILE__, __LINE__);
		return false;
	}

	return true;
}

void SceneManager::AddScene(Scene* newScene_){
	//Make sure the Scene is a valid pointer
	_ASSERT(newScene_ != nullptr);

	sceneList.push_back(newScene_);
}

bool SceneManager::LoadInitialScene(){
	//Make sure NO scene is currently loaded
	_ASSERT(currentSceneIndex == -1);
	//Make sure there's at least one scene in the scene list
	_ASSERT(sceneList.size() > 0);

	currentSceneIndex = 0;
	if(sceneList[currentSceneIndex]->Initialize() == false){
		Debug::LogError("Scene could not be initialized!", __FILE__, __LINE__);
		return false;
	}

	return true;
}

void SceneManager::LoadScene(unsigned int sceneIndex_){
	//Make sure that a scene is loaded
	//If you're hitting this assertion, you should be using LoadInitialScene
	_ASSERT(currentSceneIndex >= 0);
	//Makes sure that the sceneIndex is valid
	_ASSERT(sceneIndex_ < sceneList.size());
	//Redundant, but check that there's at least one scene in the scene list
	_ASSERT(!sceneList.empty());

	sceneLoadQueue.push(sceneIndex_);
}

void SceneManager::LoadNextScene(){
	_ASSERT(currentSceneIndex >= 0);
	_ASSERT(!sceneList.empty());

	if(static_cast<size_t>(currentSceneIndex + 1) >= sceneList.size()){
		Debug::LogWarning("The final scene in the scene list is currently loaded, cannot load next scene!", __FILE__, __LINE__);
		return;
	}

	sceneLoadQueue.push(currentSceneIndex + 1);
}

void SceneManager::ReloadCurrentScene(){
	_ASSERT(currentSceneIndex >= 0);
	_ASSERT(!sceneList.empty());

	sceneLoadQueue.push(currentSceneIndex);
}

Scene* SceneManager::CurrentScene(){
	//Make sure there's at least one scene in the Scene list
	_ASSERT(sceneList.size() > 0);
	//Make sure that a scene is loaded
	//If you're hitting this assertion, make sure you're using LoadInitialScene somewhere
	_ASSERT(currentSceneIndex >= 0);

	return sceneList[currentSceneIndex];
}

int SceneManager::CurrentSceneIndex(){
	return currentSceneIndex;
}

unsigned int SceneManager::NumScenes(){
	return static_cast<unsigned int>(sceneList.size());
}

bool SceneManager::TestLoadingAllScenes(){
	//Make sure NO scene is currently loaded
	//You should only use this BEFORE actually loading any scenes
	_ASSERT(currentSceneIndex == -1);

	for(unsigned int i = 0; i < sceneList.size(); i++){
		if(sceneList[i]->Initialize() == false){
			Debug::LogError("Scene at index " + std::to_string(i) + " failed to initialize!", __FILE__, __LINE__);
			return false;
		}

		Debug::Log("Scene " + std::to_string(i) + " initialized successfully", __FILE__, __LINE__);
		sceneList[i]->Destroy();
	}

	return true;
}

bool SceneManager::ChangeToNewScene(){
	unsigned int newIndex = sceneLoadQueue.front();

	//Make sure that a scene is loaded
	//If you're hitting this assertion, you should be using LoadInitialScene
	_ASSERT(currentSceneIndex >= 0);
	//Makes sure that the sceneIndex is valid
	_ASSERT(newIndex < sceneList.size());
	//Redundant, but check that there's at least one scene in the scene list
	_ASSERT(sceneList.size() > 0);

	//Unload the current scene
	sceneList[currentSceneIndex]->Destroy();

	UIManager::DisableAllSets(); //Disable all UI elements when we change scenes
	Debug::OnSceneChange();

	//Set the current scene index and load the current scene
	currentSceneIndex = newIndex;
	if(sceneList[currentSceneIndex]->Initialize() == false){
		Debug::LogError("Scene could not be initialized!", __FILE__, __LINE__);
		GameManager::Stop();
		return false;
	}

	return true;
}