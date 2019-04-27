#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <map>
#include <vector>

#include "Core/ScreenCoordinate.h"
#include "Input/Axis.h"
#include "Input/Button.h"
#include "Input/Gamepad.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

namespace PizzaBox{
	class InputManager{
	public:
		static bool Initialize();
		static void Destroy();

		static void PollInput();

		static bool GetButtonDown(const std::string& button_);
		static bool GetButtonUp(const std::string& button_);
		static bool GetButtonHeld(const std::string& button_);
		static float GetAxis(const std::string& axis_);

		static void AddButton(const std::string& name_, Button* button);
		static void AddAxis(const std::string& name_, Axis* axis);

		static bool GetKeyDown(SDL_Keycode key_);
		static bool GetKeyUp(SDL_Keycode key_);
		static bool GetKeyHeld(SDL_Keycode key_);

		static bool GetMouseDown(Uint8 button_);
		static bool GetMouseUp(Uint8 button_);
		static bool GetMouseHeld(Uint8 button_);
		static ScreenCoordinate GetMousePosition();
		static MouseDelta GetMouseDelta();
		static int GetMouseWheelX();
		static int GetMouseWheelY();

		static short GetGamepadAxis(unsigned int gamepad_, unsigned int axis_);
		static Uint8 GetGamepadDPad(unsigned int gamepad_, unsigned int padIndex_ = 0);
		static bool GetGamepadButtonDown(unsigned int gamepad_, unsigned int button_);
		static bool GetGamepadButtonUp(unsigned int gamepad_, unsigned int button_);
		static bool GetGamepadButtonHeld(unsigned int gamepad_, unsigned int button_);

		//Destroy unwanted compiler generated constructors, destructor and assignment operators
		InputManager() = delete;
		InputManager(const InputManager&) = delete;
		InputManager(InputManager&&) = delete;
		InputManager& operator=(const InputManager&) = delete;
		InputManager& operator=(InputManager&&) = delete;
		~InputManager() = delete;

	private:
		static Keyboard* keyboard;
		static Mouse* mouse;
		static std::vector<Gamepad*> gamepads;

		static std::map<std::string, Button*> buttons;
		static std::map<std::string, Axis*> axes;

		static bool AddGamepad(unsigned int index_);
		static void RemoveGamepad(unsigned int index_);
		static Gamepad* GetGamepadByID(unsigned int id_);
	};
}

#endif //!INPUT_MANAGER_H