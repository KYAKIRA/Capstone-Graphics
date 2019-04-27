#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <vector>
#include <set>

#include <SDL.h>

namespace PizzaBox{
	struct Gamepad{
		Gamepad();
		~Gamepad();

		SDL_Joystick* joystick;
		std::vector<short> axes;
		std::vector<Uint8> pads;

		SDL_JoystickID id;
		Uint8 numButtons;
		std::set<Uint8> buttonsDown;
		std::set<Uint8> buttonsUp;
		std::set<Uint8> buttonsHeld;

		bool Open(unsigned int index_);
		void Close();

		void SetAxisValue(unsigned int axis_);
		void SetPadValue(unsigned int pad_);

		void Reset();
		void AddButtonDown(Uint8 button_);
		void AddButtonUp(Uint8 button_);
		void AddButtonHeld(Uint8 button_);
		void RemoveButtonHeld(Uint8 button_);
	};
}

#endif //!GAMEPAD_H