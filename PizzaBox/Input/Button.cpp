#include "Button.h"

#include "InputManager.h"

using namespace PizzaBox;

Button::Button() : key(nullptr), mouse(nullptr), gamepadIndex(0), gamepad(nullptr){
}

Button::~Button(){
	if(key != nullptr){
		delete key;
		key = nullptr;
	}

	if(mouse != nullptr){
		delete mouse;
		mouse = nullptr;
	}

	if(gamepad != nullptr){
		delete gamepad;
		gamepad = nullptr;
	}
}

void Button::SetKeyInput(SDL_Keycode key_){
	key = new SDL_Keycode(key_);
}

void Button::SetMouseInput(Uint8 mouse_){
	mouse = new Uint8(mouse_);
}

void Button::SetGamepadInput(unsigned int gamepadIndex_, Uint8 gamepad_){
	gamepadIndex = gamepadIndex_;
	gamepad = new Uint8(gamepad_);
}

bool Button::GetDown(){
	if(key != nullptr){
		if(InputManager::GetKeyDown(*key)){
			return true;
		}
	}

	if(mouse != nullptr){
		if(InputManager::GetMouseDown(*mouse)){
			return true;
		}
	}

	if(gamepad != nullptr){
		if(InputManager::GetGamepadButtonDown(gamepadIndex, *gamepad)){
			return true;
		}
	}

	return false;
}

bool Button::GetUp(){
	if(key != nullptr){
		if(InputManager::GetKeyUp(*key)){
			return true;
		}
	}

	if(mouse != nullptr){
		if(InputManager::GetMouseUp(*mouse)){
			return true;
		}
	}

	if(gamepad != nullptr){
		if(InputManager::GetGamepadButtonUp(gamepadIndex, *gamepad)){
			return true;
		}
	}

	return false;
}

bool Button::GetHeld(){
	if(key != nullptr){
		if(InputManager::GetKeyHeld(*key)){
			return true;
		}
	}

	if(mouse != nullptr){
		if(InputManager::GetMouseHeld(*mouse)){
			return true;
		}
	}

	if(gamepad != nullptr){
		if(InputManager::GetGamepadButtonHeld(gamepadIndex, *gamepad)){
			return true;
		}
	}

	return false;
}