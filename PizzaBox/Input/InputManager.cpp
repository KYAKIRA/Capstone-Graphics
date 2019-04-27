#include "InputManager.h"

#include <rttr/registration.h>
#include <SDL.h>

#include "InputParser.h"
#include "Core/GameManager.h"
#include "Graphics/RenderEngine.h"

using namespace PizzaBox;

//Initialize static variables
Keyboard* InputManager::keyboard = nullptr;
Mouse* InputManager::mouse = nullptr;
std::vector<Gamepad*> InputManager::gamepads;
std::map<std::string, Button*> InputManager::buttons;
std::map<std::string, Axis*> InputManager::axes;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<InputManager>("InputManager")
		.method("Initialize", &InputManager::Initialize)
		.method("Destroy", &InputManager::Destroy)
		.method("PollInput", &InputManager::PollInput)
		.method("GetButtonDown", &InputManager::GetButtonDown)
		.method("GetButtonUp", &InputManager::GetButtonUp)
		.method("GetButtonHeld", &InputManager::GetButtonHeld)
		.method("GetAxis", &InputManager::GetAxis)
		.method("AddButton", &InputManager::AddButton)
		.method("AddAxis", &InputManager::AddAxis)
		.method("GetKeyDown", &InputManager::GetKeyDown)
		.method("GetKeyUp", &InputManager::GetKeyUp)
		.method("GetKeyHeld", &InputManager::GetKeyHeld)
		.method("GetMouseDown", &InputManager::GetMouseDown)
		.method("GetMouseUp", &InputManager::GetMouseUp)
		.method("GetMouseHeld", &InputManager::GetMouseHeld)
		.method("GetMousePosition", &InputManager::GetMousePosition)
		.method("GetMouseDelta", &InputManager::GetMouseDelta)
		.method("GetMouseWheelX", &InputManager::GetMouseWheelX)
		.method("GetMouseWheelY", &InputManager::GetMouseWheelY)
		.method("GetGamepadAxis", &InputManager::GetGamepadAxis)
		.method("GetGamepadDPad", &InputManager::GetGamepadDPad)
		.method("GetGamepadButtonDown", &InputManager::GetGamepadButtonDown)
		.method("GetGamepadButtonUp", &InputManager::GetGamepadButtonUp)
		.method("GetGamepadButtonHeld", &InputManager::GetGamepadButtonHeld);
}
#pragma warning( pop )

bool InputManager::Initialize(){
	_ASSERT(keyboard == nullptr);
	_ASSERT(mouse == nullptr);
	_ASSERT(gamepads.empty());
	//_ASSERT(buttons.empty());
	//_ASSERT(axes.empty());

	if(InputParser::ParseInputBinds("Resources/DefaultInputs.ini", "InputConfig.ini") == false){
		Debug::LogError("Could not parse bound inputs!", __FILE__, __LINE__);
		return false;
	}

	keyboard = new Keyboard();
	mouse = new Mouse();
	return true;
}

void InputManager::Destroy(){
	axes.clear();
	buttons.clear();
	
	for(unsigned int i = 0; i < gamepads.size(); i++){
		RemoveGamepad(i);
	}
	gamepads.clear();
	gamepads.shrink_to_fit();

	if(mouse != nullptr){
		delete mouse;
		mouse = nullptr;
	}

	if(keyboard != nullptr){
		delete keyboard;
		keyboard = nullptr;
	}
}

//Check all input events
void InputManager::PollInput(){
	//Buttons down and up should only be active for one frame
	//So we reset them every time we call this function
	keyboard->Reset();
	mouse->Reset();
	for(Gamepad* gp : gamepads){
		gp->Reset();
	}

	Gamepad* gp = nullptr;

	SDL_Event event;
	while(SDL_PollEvent(&event)){
		switch(event.type){
			case SDL_QUIT:
				GameManager::Stop(); //If a Quit event is triggered, end the game
				break;
			case SDL_KEYDOWN:
				keyboard->AddKeyDown(event.key.keysym.sym);
				keyboard->AddKeyHeld(event.key.keysym.sym);
				break;
			case SDL_KEYUP:
				keyboard->AddKeyUp(event.key.keysym.sym);
				keyboard->RemoveKeyHeld(event.key.keysym.sym);
				break;
			case SDL_MOUSEMOTION:
				if(SDL_GetRelativeMouseMode() == SDL_TRUE){
					mouse->SetDelta(MouseDelta(event.motion.xrel, event.motion.yrel));
				}else{
					int mouseX, mouseY;
					SDL_GetMouseState(&mouseX, &mouseY); //Gives us the X and Y position of the mouse
					mouse->SetPosition(ScreenCoordinate(static_cast<unsigned int>(mouseX), static_cast<unsigned int>(mouseY)));
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse->AddButtonDown(event.button.button);
				mouse->AddButtonHeld(event.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				mouse->AddButtonUp(event.button.button);
				mouse->RemoveButtonHeld(event.button.button);
				break;
			case SDL_MOUSEWHEEL:
				mouse->SetWheelX(event.wheel.x);
				mouse->SetWheelY(event.wheel.y);
				break;
			case SDL_JOYDEVICEADDED:
				if(AddGamepad(event.jdevice.which) == false){
					Debug::DisplayFatalErrorMessage("Input Error!", "Gamepad could not be connected! SDL Error: " + std::string(SDL_GetError()));
					SDL_ClearError();
					GameManager::Stop();
					break;
				}
				Debug::Log("Gamepad " + std::to_string(gamepads.back()->id) + " connected successfully", __FILE__, __LINE__);
				break;
			case SDL_JOYDEVICEREMOVED:
				RemoveGamepad(event.jdevice.which);
				Debug::Log("Gamepad " + std::to_string(event.jdevice.which) + " disconnected successfully", __FILE__, __LINE__);
				break;
			case SDL_JOYBUTTONDOWN:
				gp = GetGamepadByID(event.jbutton.which);
				if(gp != nullptr){
					gp->AddButtonDown(event.jbutton.button);
					gp->AddButtonHeld(event.jbutton.button);
				}
				break;
			case SDL_JOYBUTTONUP:
				gp = GetGamepadByID(event.jbutton.which);
				if(gp != nullptr){
					gp->AddButtonUp(event.jbutton.button);
					gp->RemoveButtonHeld(event.jbutton.button);
				}
				break;
			case SDL_JOYAXISMOTION:
				gp = GetGamepadByID(event.jaxis.which);
				if(gp != nullptr){
					gp->SetAxisValue(event.jaxis.axis);
				}
				break;
			case SDL_JOYHATMOTION:
				gp = GetGamepadByID(event.jhat.which);
				if(gp != nullptr){
					gp->SetPadValue(event.jhat.hat);
				}
				break;
			case SDL_WINDOWEVENT:
				if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
					RenderEngine::OnResize(event.window.data1, event.window.data2);
				}
				break;
			default:
				break;
		}
	}
}

bool InputManager::GetButtonDown(const std::string& button_){
	auto search = buttons.find(button_);
	if(search == buttons.end()){
		Debug::LogError("Attempted to access a button that hasn't been set up! from " + button_, __FILE__, __LINE__);
		return false;
	}

	return search->second->GetDown();
}

bool InputManager::GetButtonUp(const std::string& button_){
	auto search = buttons.find(button_);
	if(search == buttons.end()){
		Debug::LogError("Attempted to access a button that hasn't been set up! from " + button_, __FILE__, __LINE__);
		return false;
	}

	return search->second->GetUp();
}

bool InputManager::GetButtonHeld(const std::string& button_){
	auto search = buttons.find(button_);
	if(search == buttons.end()){
		Debug::LogError("Attempted to access a button that hasn't been set up! from " + button_, __FILE__, __LINE__);
		return false;
	}

	return search->second->GetHeld();
}

float InputManager::GetAxis(const std::string& axis_){
	auto search = axes.find(axis_);
	if(search == axes.end()){
		Debug::LogError("Attempted to access an axis that hasn't been set up! from " + axis_, __FILE__, __LINE__);
		return false;
	}

	return search->second->GetAxis();
}

void InputManager::AddButton(const std::string& name_, Button* button_){
	_ASSERT(button_ != nullptr);

	//Try to find a resource with this name in the map
	auto search = buttons.find(name_);

	//This assertion will trigger if a resource with this name is already in the map
	_ASSERT(search == buttons.end());

	if(search == buttons.end()){
		//If there isn't a resource with this name in the map then create one
		buttons.insert(std::make_pair(name_, button_));
	}
}

void InputManager::AddAxis(const std::string& name_, Axis* axis_){
	_ASSERT(axis_ != nullptr);

	//Try to find a resource with this name in the map
	auto search = axes.find(name_);

	//This assertion will trigger if a resource with this name is already in the map
	_ASSERT(search == axes.end());

	if(search == axes.end()){
		//If there isn't a resource with this name in the map then create one
		axes.insert(std::make_pair(name_, axis_));
	}
}

bool InputManager::GetKeyDown(SDL_Keycode key_){
	//If the inputted key is in the list of keysDown, then that key is down
	return keyboard->keysDown.find(key_) != keyboard->keysDown.end();
}

bool InputManager::GetKeyUp(SDL_Keycode key_){
	//If the inputted key is in the list of keysUp, then that key is up
	return keyboard->keysUp.find(key_) != keyboard->keysUp.end();
}

bool InputManager::GetKeyHeld(SDL_Keycode key_){
	//If the inputted key is in the list of keysHeld, then that key is being held
	return keyboard->keysHeld.find(key_) != keyboard->keysHeld.end();
}

bool InputManager::GetMouseDown(Uint8 button_){
	//If the inputted button is in the list of buttons down, then that button is down
	return mouse->buttonsDown.find(button_) != mouse->buttonsDown.end();
}

bool InputManager::GetMouseUp(Uint8 button_){
	//If the inputted button is in the list of buttons up, then that button is up
	return mouse->buttonsUp.find(button_) != mouse->buttonsUp.end();
}

bool InputManager::GetMouseHeld(Uint8 button_){
	//If the inputted button is in the list of buttons held, then that button is being held
	return mouse->buttonsHeld.find(button_) != mouse->buttonsHeld.end();
}

ScreenCoordinate InputManager::GetMousePosition(){
	//Returns the mouse position we calculated for this frame
	return mouse->position;
}

MouseDelta InputManager::GetMouseDelta(){
	return mouse->delta;
}

int InputManager::GetMouseWheelX(){
	return mouse->wheelX;
}

int InputManager::GetMouseWheelY(){
	return mouse->wheelY;
}

short InputManager::GetGamepadAxis(unsigned int gamepad_, unsigned int axis_){
	if(gamepad_ >= gamepads.size() || gamepads[gamepad_] == nullptr){
		return 0;
	}

	if(axis_ >= gamepads[gamepad_]->axes.size()){
		return 0;
	}

	short axisValue = gamepads[gamepad_]->axes[axis_];
	return axisValue;
}

Uint8 InputManager::GetGamepadDPad(unsigned int gamepad_, unsigned int padIndex_){
	if(gamepad_ >= gamepads.size() || gamepads[gamepad_] == nullptr){
		return SDL_HAT_CENTERED;
	}

	if(padIndex_ >= gamepads[gamepad_]->pads.size()){
		return SDL_HAT_CENTERED;
	}

	return gamepads[gamepad_]->pads[padIndex_];
}

bool InputManager::GetGamepadButtonDown(unsigned int gamepad_, unsigned int button_){
	if(gamepad_ >= gamepads.size() || gamepads[gamepad_] == nullptr){
		return false;
	}

	return gamepads[gamepad_]->buttonsDown.find(button_) != gamepads[gamepad_]->buttonsDown.end();
}

bool InputManager::GetGamepadButtonUp(unsigned int gamepad_, unsigned int button_){
	if(gamepad_ >= gamepads.size() || gamepads[gamepad_] == nullptr){
		return false;
	}

	return gamepads[gamepad_]->buttonsUp.find(button_) != gamepads[gamepad_]->buttonsUp.end();
}

bool InputManager::GetGamepadButtonHeld(unsigned int gamepad_, unsigned int button_){
	if(gamepad_ >= gamepads.size() || gamepads[gamepad_] == nullptr){
		return false;
	}

	return gamepads[gamepad_]->buttonsHeld.find(button_) != gamepads[gamepad_]->buttonsHeld.end();
}

bool InputManager::AddGamepad(unsigned int index_){
	Gamepad* gamepad = new Gamepad();
	if(gamepad->Open(index_) == false){
		Debug::LogError("Gamepad could not be created!", __FILE__, __LINE__);
		return false;
	}

	gamepads.push_back(gamepad);
	return true;
}

void InputManager::RemoveGamepad(unsigned int index_){
	Gamepad* gp = GetGamepadByID(index_);
	if(gp != nullptr){
		delete gp;
	}

	//Remove all nullptrs from the gamepad list
	gamepads.erase(std::remove(gamepads.begin(), gamepads.end(), gp), gamepads.end());
}

Gamepad* InputManager::GetGamepadByID(unsigned int id_){
	for(Gamepad* gp : gamepads){
		if(gp->id == id_){
			return gp;
		}
	}

	Debug::LogWarning("Attempted to access a nonexistant Gamepad!", __FILE__, __LINE__);
	return nullptr;
}