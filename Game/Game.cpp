#include "Game.h"

#include <Core/SceneManager.h>
#include <Graphics/UI/ButtonUI.h>
#include <Graphics/UI/UIManager.h>

#include "Scenes/LogoScene.h"
#include "Scenes/DefaultScene.h"
#include "Scenes/SolarScene.h"

using namespace GamePackage;

Game::Game(const std::string name_) : GameInterface(name_){
}

Game::~Game(){
}

bool Game::Initialize(){ 
	//UI Set 
	/*PizzaBox::UISet* uiSet = new PizzaBox::UISet("UI");
	uiSet->elements.push_back(new PizzaBox::ButtonUI("Button", PizzaBox::Rect(0.2f, 0.2f, 0.2f, 0.2f)));
	PizzaBox::UIManager::AddSet(uiSet);*/

	//Add scenes to the SceneManager
	PizzaBox::SceneManager::AddScene(new LogoScene());
	PizzaBox::SceneManager::AddScene(new DefaultScene());
	PizzaBox::SceneManager::AddScene(new SolarScene());

	return true;
}

void Game::Destroy(){
}