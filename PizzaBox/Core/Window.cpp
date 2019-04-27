#include "Window.h"

#include "Config.h"
#include "GameManager.h"
#include "../Tools/Debug.h"

using namespace PizzaBox;

//Initialize all variables
Window::Window(const std::string name_, const unsigned int width_, const unsigned int height_, bool fullscreen_, bool borderless_) : name(name_), size(width_, height_), isFullscreen(fullscreen_), isBorderless(borderless_), sync(VSYNC::On), window(nullptr), glContext(){
	//Why the hell are you making a zero width / zero height window?
	_ASSERT(width_ != 0);
	_ASSERT(height_ != 0);
}

Window::Window(const std::string name_, const ScreenCoordinate& sc, bool fullscreen_, bool borderless_) : name(name_), size(sc), isFullscreen(fullscreen_), isBorderless(borderless_), sync(VSYNC::On), window(nullptr), glContext(){
	//Why the hell are you making a zero width / zero height window?
	_ASSERT(sc.x != 0);
	_ASSERT(sc.y != 0);
}

//We use Destroy to clean up memory so we don't need to do anything here
Window::~Window(){
}

bool Window::Initialize(){
	//Initialize SDL
	if(SDL_Init(SDL_INIT_EVERYTHING) > 0){
		Debug::LogError("SDL could not be initialized! SDL Error: " + std::string(SDL_GetError()), __FILE__, __LINE__);
		return false;
	}

	Uint32 windowFlag = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	if(isFullscreen){
		windowFlag |= SDL_WINDOW_FULLSCREEN;
	}
	if(isBorderless){
		windowFlag |= SDL_WINDOW_BORDERLESS;
	}

	//Create SDL Window
	window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x, size.y, windowFlag);
	if(window == nullptr){
		Debug::LogError("Window could not be created! SDL Error: " + std::string(SDL_GetError()), __FILE__, __LINE__);
		return false;
	}

	//Create the OpenGL context
	glContext = SDL_GL_CreateContext(window);
	//Setup OpenGL Attributes
	SetGLAttributes();
	
	SetVSYNC(static_cast<VSYNC>(Config::GetInt("VSYNC")));

	//Initialize GLEW
	GLenum err = glewInit();
	if(err != GLEW_OK){
		//Reinterpret cast allows us to convert a GLubyte to a const char*
		//This is unfortunately the only way to do this
		Debug::LogError("GLEW could not be initialized! GLEW Error: " + std::string(reinterpret_cast<const char*>(glewGetErrorString(err))), __FILE__, __LINE__);
		return false;
	}

	return true;
}

void Window::Destroy(){
	//The SDL/GL context depends on the window
	//So we destroy the context before the window
	SDL_GL_DeleteContext(glContext);

	//Destroy the Window
	if(window != nullptr){
		SDL_DestroyWindow(window);
		window = nullptr;
	}

	//Close the SDL Subsystem
	SDL_Quit();
}

void Window::UpdateWindow(){
	//Displays the current frame onto the Window
	SDL_GL_SwapWindow(window);
}

void Window::SetGLAttributes(){
	//Ignore deprecated functionality
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//Set the OpenGL version to 3.3 (this covers most computers)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	//Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//Set up the depth buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	//Sets up for Multi sampling/ Anti Aliasing 
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	#endif //_DEBUG
}

void Window::SetSize(const ScreenCoordinate& sc_){
	//Why the hell are you making a zero width / zero height window?
	_ASSERT(sc_.x != 0);
	_ASSERT(sc_.y != 0);
	size = sc_;
}

void Window::SetSize(unsigned int width_, unsigned int height_){
	//Why the hell are you making a zero width / zero height window?
	_ASSERT(width_ != 0);
	_ASSERT(height_ != 0);
	SetSize(ScreenCoordinate(width_, height_));
}

SDL_Window* Window::GetWindow() const{
	return window;
}

unsigned int Window::GetWidth() const{
	return size.x;
}

unsigned int Window::GetHeight() const{
	return size.y;
}

ScreenCoordinate Window::GetSize() const{
	return size;
}

bool Window::IsFullscreen() const{
	return isFullscreen;
}

void Window::SetFullscreen(bool fullscreen_){
	if(isFullscreen == fullscreen_){
		return;
	}

	isFullscreen = fullscreen_;

	Uint32 windowFlag = 0;
	if(isFullscreen){
		windowFlag = SDL_WINDOW_FULLSCREEN;
	}

	SDL_SetWindowFullscreen(window, windowFlag);
	if(!isFullscreen){
		SetBorderless(isBorderless);
	}
}

bool Window::IsBorderless() const{
	return isBorderless;
}

void Window::SetBorderless(bool borderless_){
	isBorderless = borderless_;

	SDL_bool flag;
	if(isBorderless){
		flag = SDL_FALSE;
	}else{
		flag = SDL_TRUE;
	}
	SDL_SetWindowBordered(window, flag);
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

Window::VSYNC Window::GetVSYNC() const{
	return sync;
}

void Window::SetVSYNC(VSYNC sync_){
	sync = sync_;
	if(SDL_GL_SetSwapInterval(static_cast<int>(sync)) == -1){
		Debug::LogWarning("Adaptive VSYNC not supported on this hardware! Defaulting to regular VSYNC!");
		sync = VSYNC::On;
		SDL_GL_SetSwapInterval(static_cast<int>(VSYNC::On));
	}

	Config::SetInt("VSYNC", static_cast<int>(sync));
}