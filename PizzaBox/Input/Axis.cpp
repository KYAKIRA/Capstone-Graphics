#include "Axis.h"

#include <rttr/registration.h>

#include "InputManager.h"
#include "Graphics/RenderEngine.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Axis>("Axis")
		.enumeration<Axis::AxisType>("AxisType")(
			rttr::value("OnClick", Axis::AxisType::X),
			rttr::value("OnRelease", Axis::AxisType::Y)
		)
		.constructor()
		.method("SetKeyInput", &Axis::SetKeyInput)
		.method("SetMouseAxis", static_cast<void(Axis::*)(Axis::AxisType)>(&Axis::SetMouseAxis))
		.method("SetMouseAxis", static_cast<void(Axis::*)(unsigned int)>(&Axis::SetMouseAxis))
		.method("SetScrollAxis", static_cast<void(Axis::*)(Axis::AxisType)>(&Axis::SetScrollAxis))
		.method("SetScrollAxis", static_cast<void(Axis::*)(unsigned int)>(&Axis::SetScrollAxis))
		.method("SetGamepadAxis", &Axis::SetGamepadAxis)
		.method("SetGamepadPaed", static_cast<void(Axis::*)(unsigned int, Axis::AxisType, unsigned int)>(&Axis::SetGamepadPad))
		.method("SetGamepadPaed", static_cast<void(Axis::*)(unsigned int, unsigned int, unsigned int)>(&Axis::SetGamepadPad))
		.method("GetAxis", &Axis::GetAxis);
}
#pragma warning( pop )

Axis::Axis() : negativeKey(nullptr), positiveKey(nullptr), mouse(nullptr), scrollWheel(nullptr), gamepadIndex(0), gamepadDeadZone(8000), gamepadAxis(nullptr), padType(nullptr), padIndex(nullptr), oldMousePos(){
}

Axis::~Axis(){
	if(negativeKey != nullptr){
		delete negativeKey;
		negativeKey = nullptr;
	}

	if(positiveKey != nullptr){
		delete positiveKey;
		positiveKey = nullptr;
	}

	if(mouse != nullptr){
		delete mouse;
		mouse = nullptr;
	}

	if(scrollWheel != nullptr){
		delete scrollWheel;
		scrollWheel = nullptr;
	}

	if(gamepadAxis != nullptr){
		delete gamepadAxis;
		gamepadAxis = nullptr;
	}

	if(padIndex != nullptr){
		delete padIndex;
		padIndex = nullptr;
	}
}

void Axis::SetKeyInput(SDL_Keycode positive_, SDL_Keycode negative_){
	#ifdef _DEBUG
	if(positive_ == negative_){
		Debug::LogWarning("Positive and negative keys are the same, axis value will always be zero!", __FILE__, __LINE__);
	}
	#endif //_DEBUG


	negativeKey = new SDL_Keycode(negative_);
	positiveKey = new SDL_Keycode(positive_);
}

void Axis::SetMouseAxis(AxisType type_){
	mouse = new AxisType(type_);
}

void Axis::SetMouseAxis(unsigned int type_){
	mouse = new AxisType(static_cast<AxisType>(type_));
}

void Axis::SetScrollAxis(AxisType type_){
	scrollWheel = new AxisType(type_);
}

void Axis::SetScrollAxis(unsigned int type_){
	scrollWheel = new AxisType(static_cast<AxisType>(type_));
}

void Axis::SetGamepadAxis(unsigned int gamepadIndex_, unsigned int axis_, float deadzone){
	_ASSERT(deadzone > 0.0f);
	_ASSERT(deadzone < 1.0f);

	gamepadIndex = gamepadIndex_;
	gamepadDeadZone = deadzone;
	gamepadAxis = new unsigned int(axis_);
}

void Axis::SetGamepadPad(unsigned int gamepadIndex_, AxisType type_, unsigned int pad_){
	gamepadIndex = gamepadIndex_;
	padType = new AxisType(type_);
	padIndex = new unsigned int(pad_);
}

void Axis::SetGamepadPad(unsigned int gamepadIndex_, unsigned int type_, unsigned int pad_){
	gamepadIndex = gamepadIndex_;
	padType = new AxisType(static_cast<AxisType>(type_));
	padIndex = new unsigned int(pad_);
}

float Axis::GetAxis(){
	if(negativeKey != nullptr && positiveKey != nullptr){
		bool posValue = InputManager::GetKeyHeld(*positiveKey);
		bool negValue = InputManager::GetKeyHeld(*negativeKey);

		if(posValue && !negValue){
			return 1.0f;
		}else if(!posValue && negValue){
			return -1.0f;
		}
	}

	if(mouse != nullptr){
		ScreenCoordinate screenSize = RenderEngine::ScreenSize();
		MouseDelta mouseDelta = InputManager::GetMouseDelta();

		if(*mouse == AxisType::X && mouseDelta.x != 0){
			return static_cast<float>(mouseDelta.x) / static_cast<float>(screenSize.x);
		}else if(*mouse == AxisType::Y && mouseDelta.y != 0){
			return static_cast<float>(mouseDelta.y) / static_cast<float>(screenSize.y);
		}
	}

	if(scrollWheel != nullptr){
		int wheelValue = 0;

		//Calculate mouse scroll wheel
		if(*scrollWheel == AxisType::X){
			wheelValue = InputManager::GetMouseWheelX();
		}else{
			wheelValue = InputManager::GetMouseWheelY();
		}

		if(wheelValue != 0){
			return static_cast<float>(wheelValue);
		}
	}

	if(gamepadAxis != nullptr){
		short axisValue = InputManager::GetGamepadAxis(gamepadIndex, *gamepadAxis);
		float f = static_cast<float>(axisValue) / 32'768;
		if(f > gamepadDeadZone || f < -gamepadDeadZone){
			return f;
		}
	}

	if(padIndex != nullptr){
		Uint8 padValue = InputManager::GetGamepadDPad(gamepadIndex, *padIndex);
		if(*padType == AxisType::X){
			switch(padValue){
				case SDL_HAT_LEFT:
				case SDL_HAT_LEFTUP:
				case SDL_HAT_LEFTDOWN:
					return -1.0f;
					break;
				case SDL_HAT_RIGHT:
				case SDL_HAT_RIGHTUP:
				case SDL_HAT_RIGHTDOWN:
					return 1.0f;
					break;
				default:
					break;
			}
		}else if(*padType == AxisType::Y){
			switch(padValue){
				case SDL_HAT_UP:
				case SDL_HAT_LEFTUP:
				case SDL_HAT_RIGHTUP:
					return 1.0f;
					break;
				case SDL_HAT_DOWN:
				case SDL_HAT_LEFTDOWN:
				case SDL_HAT_RIGHTDOWN:
					return -1.0f;
					break;
				default:
					break;
			}
		}
	}

	return 0.0f;
}