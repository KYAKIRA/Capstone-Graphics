#include "LogoScene.h"

#include <Graphics/Camera.h>
#include <Graphics/UI/UIManager.h>
#include <Graphics/UI/ImageUI.h>
#include <Graphics/RenderEngine.h>

#include "../Scripts/LogoManager.h" 

using namespace GamePackage;

LogoScene::LogoScene() : Scene(){
}
 
LogoScene::~LogoScene(){
}

bool LogoScene::Initialize(){
	//Skybox
	//SetSky(new PizzaBox::SkyBox("LakeSkybox", "SkyBoxShader", 1000.0f));

	//Create all GameObjects for this scene
	PizzaBox::GameObject* mainCamera = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(0.0f, 0.0f, 20.0f));
	mainCamera->AddComponent(new PizzaBox::Camera(PizzaBox::ViewportRect::fullScreen, PizzaBox::Camera::RenderMode::Perspective));
	//mainCamera->AddComponent(new PizzaBox::AudioListener());  

	//UI
	PizzaBox::UISet* uiSet = new PizzaBox::UISet("UI");
	PizzaBox::ImageUI* logo = new PizzaBox::ImageUI("Logo", "LogoImage", PizzaBox::Rect(0.0f, 0.0f, 1.0f, 1.0f));
	uiSet->elements.push_back(logo);
	PizzaBox::UIManager::AddSet(uiSet);
	PizzaBox::UIManager::EnableSet("UI");

	PizzaBox::GameObject* logoManager = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(0.0f, 0.0f, 0.0f));
	logoManager->AddComponent(new LogoManager(logo, 5.0f, 1.0f));
	
	return true;
}

void LogoScene::Destroy(){
	Scene::Destroy();
}