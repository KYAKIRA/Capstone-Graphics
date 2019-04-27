#include "Mouse.h"

#include "../Tools/Debug.h"

using namespace PizzaBox;

Mouse::Mouse() : position(ScreenCoordinate()), delta(MouseDelta(0, 0)), wheelX(0), wheelY(0), buttonsDown(std::set<Uint8>()), buttonsUp(std::set<Uint8>()), buttonsHeld(std::set<Uint8>()){
}

Mouse::~Mouse(){
	buttonsDown.clear();
	buttonsUp.clear();
	buttonsHeld.clear();
}

void Mouse::SetPosition(const ScreenCoordinate& pos){
	delta = MouseDelta(pos.x - position.x, pos.y - position.y);
	position = pos;
}

void Mouse::SetDelta(const MouseDelta& delta_){
	delta = delta_;
}

void Mouse::SetWheelX(const int wheel_){
	wheelX = wheel_;
}

void Mouse::SetWheelY(const int wheel_){
	wheelY = wheel_;
}

void Mouse::Reset(){
	buttonsDown.clear();
	buttonsUp.clear();
	wheelX = 0;
	wheelY = 0;
	delta = MouseDelta(0, 0);
}

void Mouse::AddButtonDown(const Uint8 button_){
	#ifdef _DEBUG
		if(buttonsDown.find(button_) != buttonsDown.end()){
			Debug::LogWarning("Attempted to re-add mouse button '" + std::to_string(button_) + "' to buttonsDown list!", __FILE__, __LINE__);
			return;
		}
	#endif //_DEBUG

	buttonsDown.insert(button_);
}

void Mouse::AddButtonUp(const Uint8 button_){
	#ifdef _DEBUG
		if(buttonsUp.find(button_) != buttonsUp.end()){
			Debug::LogWarning("Attempted to re-add mouse button '" + std::to_string(button_) + "' to buttonsUp list!", __FILE__, __LINE__);
			return;
		}
	#endif //_DEBUG

	buttonsUp.insert(button_);
}

void Mouse::AddButtonHeld(const Uint8 button_){
	#ifdef _DEBUG
		if(buttonsHeld.find(button_) != buttonsHeld.end()){
			Debug::LogWarning("Attempted to re-add mouse button '" + std::to_string(button_) + "' to buttonsHeld list!", __FILE__, __LINE__);
			return;
		}
	#endif //_DEBUG

	buttonsHeld.insert(button_);
}

void Mouse::RemoveButtonHeld(const Uint8 button_){
	#ifdef _DEBUG
		if(buttonsHeld.find(button_) == buttonsHeld.end()){
			Debug::LogWarning("Attempted to remove mouse button '" + std::to_string(button_) + "' from buttonsHeld list!", __FILE__, __LINE__);
			return;
		}
	#endif //_DEBUG

	buttonsHeld.erase(button_);
}