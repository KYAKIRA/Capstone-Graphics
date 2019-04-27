#ifndef AXIS_H
#define AXIS_H

#include <SDL.h>

#include "Core/ScreenCoordinate.h"

namespace PizzaBox{
	class Axis{
	public:
		enum class AxisType{
			X = 0,
			Y = 1
		};

		Axis();
		~Axis();

		void SetKeyInput(SDL_Keycode positive_, SDL_Keycode negative_);
		void SetMouseAxis(AxisType type_);
		void SetMouseAxis(unsigned int type_);
		void SetScrollAxis(AxisType type_);
		void SetScrollAxis(unsigned int type_);
		void SetGamepadAxis(unsigned int gamepadIndex_, unsigned int axis_, float deadzone = 0.25f);
		void SetGamepadPad(unsigned int gamepadIndex_, AxisType type_, unsigned int pad_ = 0);
		void SetGamepadPad(unsigned int gamepadIndex_, unsigned int type_, unsigned int pad_ = 0);

		float GetAxis();

	private:
		SDL_Keycode* negativeKey;
		SDL_Keycode* positiveKey;
		AxisType* mouse;
		AxisType* scrollWheel;
		unsigned int gamepadIndex;
		float gamepadDeadZone;
		unsigned int* gamepadAxis;
		AxisType* padType;
		unsigned int* padIndex;

		ScreenCoordinate oldMousePos;
	};
}

#endif //!AXIS_H