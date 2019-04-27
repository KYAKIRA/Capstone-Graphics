#include "TrackballControlled.h"

#include <Graphics/RenderEngine.h>
#include <Input/InputManager.h>
#include <Object/GameObject.h>

using namespace GamePackage;

TrackballControlled::TrackballControlled() : PizzaBox::Script(), isActive(false), trackball(nullptr), screenSize(), lastMousePos(){
}

TrackballControlled::~TrackballControlled(){
}

void TrackballControlled::OnStart(){
	screenSize = PizzaBox::RenderEngine::ScreenSize();
	trackball = new PizzaBox::Trackball(screenSize);
	Script::OnStart();
}

void TrackballControlled::Update(const float deltaTime){
	//Checking this every frame is SUPER inefficient, but it'll be fine for now
	if(screenSize != PizzaBox::RenderEngine::ScreenSize()){
		screenSize = PizzaBox::RenderEngine::ScreenSize();
		trackball->SetWindowDimensions(screenSize);
	}

	if(PizzaBox::InputManager::GetMouseDown(SDL_BUTTON_LEFT)){
		lastMousePos = PizzaBox::InputManager::GetMousePosition();
		isActive = true;
		trackball->OnLeftMouseDown(lastMousePos);
	}

	if(PizzaBox::InputManager::GetMouseUp(SDL_BUTTON_LEFT)){
		isActive = false;
	}

	if(isActive){
		//If the mouse position has changed, update the trackball
		if(lastMousePos != PizzaBox::InputManager::GetMousePosition()){
			lastMousePos = PizzaBox::InputManager::GetMousePosition();

			trackball->OnMouseMove(lastMousePos);

			gameObject->GetTransform()->Rotate(trackball->GetMatrix3());
		}
	}
}

void TrackballControlled::OnDestroy(){
	if(trackball != nullptr){
		delete trackball;
		trackball = nullptr;
	}
}