#include "Gamepad.h"

#include "Tools/Debug.h"

using namespace PizzaBox;

Gamepad::Gamepad() : joystick(), axes(), pads(), id(), numButtons(0), buttonsDown(), buttonsUp(), buttonsHeld(){
}

Gamepad::~Gamepad(){
	Close();

	axes.clear();
	axes.shrink_to_fit();
	
	pads.clear();
	pads.shrink_to_fit();
	
	buttonsDown.clear();
	buttonsUp.clear();
	buttonsHeld.clear();
}

bool Gamepad::Open(unsigned int index_){
	joystick = SDL_JoystickOpen(index_);
	if(joystick == nullptr){
		Debug::LogError("Joystick could not be opened! SDL Error: " + std::string(SDL_GetError()), __FILE__, __LINE__);
		SDL_ClearError();
		return false;
	}

	id = SDL_JoystickInstanceID(joystick); //Returns -1 on failure
	if(id < 0){
		Debug::LogError("Could not get JoystickInstanceID! SDL Error: " + std::string(SDL_GetError()));
		SDL_ClearError();
		return false;
	}

	int numAxes = SDL_JoystickNumAxes(joystick); //Returns -1 on failure
	if(numAxes < 0){
		Debug::LogError("Could not get the number of axes for this joystick! SDL Error: " + std::string(SDL_GetError()), __FILE__, __LINE__);
		SDL_ClearError();
		return false;
	}

	int numPads = SDL_JoystickNumHats(joystick); //Returns -1 on failure
	if(numPads < 0){
		Debug::LogError("Could not get the number of hats for this joystick! SDL Error: " + std::string(SDL_GetError()), __FILE__, __LINE__);
		SDL_ClearError();
		return false;
	}

	numButtons = SDL_JoystickNumButtons(joystick); //Returns -1 on failure
	if(numButtons < 0){
		Debug::LogError("Could not get the number of buttons for this joystick! SDL Error: " + std::string(SDL_GetError()), __FILE__, __LINE__);
		SDL_ClearError();
		return false;
	}

	axes = std::vector<short>(numAxes);
	std::fill(axes.begin(), axes.end(), 0);
	axes.shrink_to_fit();

	pads = std::vector<Uint8>(numPads);
	std::fill(pads.begin(), pads.end(), 0);
	pads.shrink_to_fit();

	return true;
}

void Gamepad::Close(){
	if(SDL_JoystickGetAttached(joystick) == SDL_TRUE){
		SDL_JoystickClose(joystick);
	}

	joystick = nullptr;
}

void Gamepad::SetAxisValue(unsigned int axis_){
	if(axis_ >= axes.size()){
		Debug::LogWarning("Axis index is invalid!", __FILE__, __LINE__);
		return;
	}

	SDL_ClearError(); //Clear SDL_Error in case there's an old error message - TODO, make this irrelevant by always clearing the last error

	short value = SDL_JoystickGetAxis(joystick, axis_);
	if(value == 0 && !std::string(SDL_GetError()).empty()){
		Debug::LogError("Could not get axis value! SDL Error: " + std::string(SDL_GetError()));
		SDL_ClearError();
	}

	axes[axis_] = value;
}

void Gamepad::SetPadValue(unsigned int pad_){
	if(pad_ >= pads.size()){
		Debug::LogWarning("Pad index is invalid!", __FILE__, __LINE__);
		return;
	}

	pads[pad_] = SDL_JoystickGetHat(joystick, pad_);
	/*if(pads[pad_] == SDL_HAT_CENTERED && !std::string(SDL_GetError()).empty()){
		Debug::LogError("Could not get pad value! SDL Error: " + std::string(SDL_GetError()));
	}*/
}

void Gamepad::Reset(){
	buttonsDown.clear();
	buttonsUp.clear();
}

void Gamepad::AddButtonDown(Uint8 button_){
	#ifdef _DEBUG
		if(buttonsDown.find(button_) != buttonsDown.end()){
			Debug::LogWarning("Attempted to re-add gamepad button '" + std::to_string(button_) + "' to buttonsDown list!");
			return;
		}
	#endif //_DEBUG

	buttonsDown.insert(button_);
}

void Gamepad::AddButtonUp(Uint8 button_){
	#ifdef _DEBUG
		if(buttonsUp.find(button_) != buttonsUp.end()){
			Debug::LogWarning("Attempted to re-add gamepad button '" + std::to_string(button_) + "' to buttonsUp list!", __FILE__, __LINE__);
			return;
		}
	#endif //_DEBUG

	buttonsUp.insert(button_);
}

void Gamepad::AddButtonHeld(Uint8 button_){
	#ifdef _DEBUG
		if(buttonsHeld.find(button_) != buttonsHeld.end()){
			Debug::LogWarning("Attempted to re-add gamepad button '" + std::to_string(button_) + "' to buttonsHeld list!", __FILE__, __LINE__);
			return;
		}
	#endif //_DEBUG

	buttonsHeld.insert(button_);
}

void Gamepad::RemoveButtonHeld(Uint8 button_){
	#ifdef _DEBUG
		if(buttonsHeld.find(button_) == buttonsHeld.end()){
			Debug::LogWarning("Attempted to remove gamepad button '" + std::to_string(button_) + "' from buttonsHeld list!", __FILE__, __LINE__);
			return;
		}
	#endif //_DEBUG

	buttonsHeld.erase(button_);
}