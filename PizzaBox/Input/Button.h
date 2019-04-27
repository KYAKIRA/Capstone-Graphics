#ifndef BUTTON_H
#define BUTTON_H

#include <string>

#include <SDL.h>

namespace PizzaBox{
	class Button{
	public:
		Button();
		~Button();

		void SetKeyInput(SDL_Keycode key_);
		void SetMouseInput(Uint8 mouse_);
		void SetGamepadInput(unsigned int gamepadIndex_, Uint8 gamepad_);

		bool GetDown();
		bool GetUp();
		bool GetHeld();

	private:
		SDL_Keycode* key;
		Uint8* mouse;
		int gamepadIndex;
		Uint8* gamepad;
	};
}

#endif //!BUTTON_H