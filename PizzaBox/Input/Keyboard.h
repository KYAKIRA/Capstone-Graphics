#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <set>

#include <SDL.h>

namespace PizzaBox{
	struct Keyboard{
		std::set<SDL_Keycode> keysDown;
		std::set<SDL_Keycode> keysUp;
		std::set<SDL_Keycode> keysHeld;

		Keyboard();
		~Keyboard();

		void Reset();
		void AddKeyDown(const SDL_Keycode key_);
		void AddKeyUp(const SDL_Keycode key_);
		void AddKeyHeld(const SDL_Keycode key_);
		void RemoveKeyHeld(const SDL_Keycode key_);
	};
}

#endif //!KEYBOARD_H