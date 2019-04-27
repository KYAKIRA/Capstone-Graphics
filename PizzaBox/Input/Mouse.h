#ifndef MOUSE_H
#define MOUSE_H

#include <set>

#include <SDL.h>

#include "../Core/ScreenCoordinate.h"
#include "MouseDelta.h"

namespace PizzaBox{
	struct Mouse{
		ScreenCoordinate position;
		MouseDelta delta;
		int wheelX;
		int wheelY;

		std::set<Uint8> buttonsDown;
		std::set<Uint8> buttonsUp;
		std::set<Uint8> buttonsHeld;

		Mouse();
		~Mouse();

		void SetPosition(const ScreenCoordinate& pos);
		void SetDelta(const MouseDelta& delta_);
		void SetWheelX(const int wheel_);
		void SetWheelY(const int wheel_);

		void Reset();
		void AddButtonDown(const Uint8 button_);
		void AddButtonUp(const Uint8 button_);
		void AddButtonHeld(const Uint8 button_);
		void RemoveButtonHeld(const Uint8 button_);
	};
}

#endif //!MOUSE_H