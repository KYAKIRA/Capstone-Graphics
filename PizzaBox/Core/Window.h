#ifndef WINDOW_H
#define WINDOW_H

#include <string>

#include <SDL.h>
#include <glew.h>

#include "ScreenCoordinate.h"

namespace PizzaBox{
	class Window{
	public:
		enum class VSYNC{
			Adaptive = -1,
			Off = 0,
			On = 1
		};

		explicit Window(const std::string name_ = "PizzaBox Engine", const unsigned int width_ = 800, const unsigned int height_ = 600, bool fullscreen_ = false, bool borderless_ = false);
		Window(const std::string name_, const ScreenCoordinate& sc, bool fullscreen_ = false, bool borderless_ = false);
		~Window();

		bool Initialize();
		void Destroy();

		void UpdateWindow();
		void SetSize(const ScreenCoordinate& sc_);
		void SetSize(unsigned int width_, unsigned int height_);
		SDL_Window* GetWindow() const;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		ScreenCoordinate GetSize() const;
		bool IsFullscreen() const;
		void SetFullscreen(bool fullscreen_);
		bool IsBorderless() const;
		void SetBorderless(bool borderless_);
		VSYNC GetVSYNC() const;
		void SetVSYNC(VSYNC sync_);

	private:
		std::string name;
		ScreenCoordinate size;
		bool isFullscreen;
		bool isBorderless;
		VSYNC sync;

		SDL_Window* window;
		SDL_GLContext glContext;
		void SetGLAttributes();
	};
}

#endif //!WINDOW_H