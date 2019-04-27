#include "Keyboard.h"

#include "../Tools/Debug.h"

using namespace PizzaBox;

Keyboard::Keyboard() : keysDown(std::set<SDL_Keycode>()), keysUp(std::set<SDL_Keycode>()), keysHeld(std::set<SDL_Keycode>()){
}

Keyboard::~Keyboard(){
	keysDown.clear();
	keysUp.clear();
	keysHeld.clear();
}

void Keyboard::Reset(){
	keysDown.clear();
	keysUp.clear();
}

void Keyboard::AddKeyDown(SDL_Keycode key_){
	//Only add a key that's down if it's not in the Held list
	//This prevents us from getting key repeat events from Windows
	if(keysHeld.find(key_) == keysHeld.end()){
		keysDown.insert(key_);
	}
}

void Keyboard::AddKeyUp(SDL_Keycode key_){
	keysUp.insert(key_);
}

void Keyboard::AddKeyHeld(SDL_Keycode key_){
	keysHeld.insert(key_);
}

void Keyboard::RemoveKeyHeld(SDL_Keycode key_){
	#ifdef _DEBUG
		if(keysHeld.find(key_) == keysHeld.end()){
			std::string key = SDL_GetKeyName(key_);
			Debug::LogWarning("Attempted to remove '" + key + "' key from keysUp list!", __FILE__, __LINE__);
			return;
		}
	#endif //_DEBUG

	keysHeld.erase(key_);
}