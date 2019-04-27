#include "Debug.h"

#include <csignal>
#include <fstream>
#include <iostream>

#include <rttr/registration.h> //This breaks if it goes below Windows.h
#include <Windows.h>
#include <SDL.h>
#include <glew.h>

#include "RayManager.h"
#include "Core/FileSystem.h"
#include "Graphics/Camera.h"

using namespace PizzaBox;

std::map<const std::string, Profiler*> Debug::profilers = std::map<const std::string, Profiler*>();
const HANDLE Debug::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
RayManager* Debug::rayManager = nullptr;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Debug>("Debug")
		.method("Initialize", &Debug::Initialize)
		.method("Destroy", &Debug::Destroy)
		.method("Log", static_cast<void(*)(const std::string&)>(&Debug::Log))
		.method("Log", static_cast<void(*)(const std::string&, const std::string&, int)>(&Debug::Log))
		.method("LogWarning", static_cast<void(*)(const std::string&)>(&Debug::LogWarning))
		.method("LogWarning", static_cast<void(*)(const std::string&, const std::string&, int)>(&Debug::LogWarning))
		.method("LogError", static_cast<void(*)(const std::string&)>(&Debug::LogError))
		.method("LogError", static_cast<void(*)(const std::string&, const std::string&, int)>(&Debug::LogError))
		.method("DisplayFatalErrorMessage", &Debug::DisplayFatalErrorMessage)
		.method("StartProfiling", &Debug::StartProfiling)
		.method("EndProfiling", &Debug::EndProfiling)
		.method("GetProfiler", &Debug::GetProfiler)
		.method("InitializeRays", &Debug::InitializeRays)
		.method("CastRay", &Debug::CastRay)
		.method("CastRayPoints", &Debug::CastRayPoints)
		.method("DrawRayCast", &Debug::DrawRayCast)
		.method("OnSceneChange", &Debug::DestroyRays);
		//.method("GLDebugCallback", &Debug::GLDebugCallback); //This function is wonky so we can't register it
}
#pragma warning( pop )

bool Debug::Initialize(){
#ifdef _DEBUG
	FileSystem::WriteToFile("log.txt", "PizzaBox Engine - Log\n", FileSystem::WriteType::clear);
#endif //_DEBUG

	//In the case of hardware signals that are errors, throw exceptions with relevant messages
	signal(SIGSEGV, [](int signal_){
		throw std::exception("Invalid memory access!");
	});

	signal(SIGILL, [](int signal_){
		throw std::exception("Abnormal termination triggered by abort call!");
	});

	signal(SIGABRT, [](int signal_){
		throw std::exception("Invalid program image!");
	});

	signal(SIGFPE, [](int signal_){
		throw std::exception("Floating point error!");
	});
	return true;
}

void Debug::Destroy(){
	for(auto& p : profilers){
		if(p.second->IsProfiling()){
			p.second->EndProfiling();
		}

		delete p.second;
		p.second = nullptr;
	}
	profilers.clear();

	if(rayManager != nullptr){
		delete rayManager;
		rayManager = nullptr;
	}
}

void Debug::Log(const std::string& log_, const std::string& file_, int line_){
	#ifdef _DEBUG
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
	std::string fileName = GetFileName(file_);
	std::cout << log_ << "\n\t\t\t || File:  " << fileName << ", Line: " << line_ << std::endl;
	WriteLogFile(log_, fileName, line_);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	#endif //_DEBUG
}

void Debug::Log(const std::string& log_){
	#ifdef _DEBUG
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
	std::cout << log_ << std::endl;
	WriteLogFile(log_);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	#endif //_DEBUG
}

void Debug::LogWarning(const std::string& warning_, const std::string& file_, int line_){
	#ifdef _DEBUG
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	std::string fileName = GetFileName(file_);
	std::cout << "Warning: " << warning_ << "\n\t\t\t || File:  " << fileName << ", Line: " << line_ << std::endl;
	WriteLogFile("Warning: " + warning_, fileName, line_);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	#endif //_DEBUG
}

void Debug::LogWarning(const std::string& warning_){
	#ifdef _DEBUG
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	std::cout << "Warning: " << warning_ << std::endl;
	WriteLogFile("Warning: " + warning_);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	#endif //_DEBUG
}

void Debug::LogError(const std::string& error_, const std::string& file_, int line_){
	#ifdef _DEBUG
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	std::string fileName = GetFileName(file_);
	std::cout << "Error: " << error_ << "\n\t\t\t || File:  " << fileName << ", Line: " << line_ << std::endl;
	WriteLogFile("Error: " + error_, fileName, line_);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	#endif //_DEBUG
}

void Debug::LogError(const std::string& error_){
	#ifdef _DEBUG
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	std::cout << "Error: " << error_ << std::endl;
	WriteLogFile("Error: " + error_);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	#endif //_DEBUG
}

void Debug::DisplayFatalErrorMessage(const std::string& title_, const std::string& message_){
	std::string spacedMessage = message_ + "         \n         "; //Extra spacing at the end to prevent text from getting cut off
	int success = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title_.c_str(), spacedMessage.c_str(), NULL);
	if(success != 0){
		LogError(std::string("MessageBox could not be shown. SDL Error: ") + SDL_GetError(), __FILE__, __LINE__);
		LogError(message_,__FILE__, __LINE__);
	}
	std::string fileName = GetFileName(__FILE__);
	WriteLogFile(title_ + ": " + message_, fileName, __LINE__);
}

void Debug::WriteLogFile(const std::string& message_, const std::string& file_, int line_){
#ifdef _DEBUG
	std::string newMessage = message_;
	newMessage.append("\t\t\t || File:  " + file_);
	newMessage.append(", Line: " + std::to_string(line_));

	WriteLogFile(newMessage);
#endif
}

void Debug::WriteLogFile(const std::string& message_){
#ifdef _DEBUG
	FileSystem::WriteToFile("log.txt", message_ + "\n", FileSystem::WriteType::append);
#endif
}

void Debug::StartProfiling(const std::string& profilerName_){
	if(profilers.find(profilerName_) == profilers.end()){
		//Key was not found, must create it first
		profilers.insert(std::pair<std::string, Profiler*>(profilerName_, new Profiler(profilerName_)));
	}

	if(!profilers.at(profilerName_)->IsProfiling()){
		profilers.at(profilerName_)->StartProfiling();
	}
}

double Debug::EndProfiling(const std::string& profilerName_){
	//Make sure this key exists
	_ASSERT(profilers.find(profilerName_) != profilers.end());
	//Make sure the profiler is currently running
	_ASSERT(profilers.at(profilerName_)->IsProfiling());

	double timeInMilliseconds = profilers.at(profilerName_)->EndProfiling();
	return timeInMilliseconds / 1000.0;
}

Profiler* Debug::GetProfiler(const std::string& profiler_){
	if(profilers.find(profiler_) == profilers.end()){
		return nullptr;
	}

	return profilers[profiler_];
}

bool Debug::InitializeRays(){
	if(rayManager == nullptr){
		rayManager = new RayManager();
	}

	if(rayManager->Initialize() == false){
		Debug::LogError("RayManager could not be initialized!", __FILE__, __LINE__);
		return false;
	}

	return true;
}

void Debug::CastRay(const Vector3& startPoint_, const Vector3& direction_, RayMode mode_, float length_, const Color& color_){
	if(rayManager == nullptr){
		return;
	}

	Vector3 rayEnd = startPoint_ + (direction_ * length_);
	rayManager->AddRay(RayCastRender(startPoint_, rayEnd, color_), mode_);
}

void Debug::CastRayPoints(const Vector3& startPoint_, const Vector3& endPoint_, RayMode mode_, const Color& color_){
	if(rayManager != nullptr){
		rayManager->AddRay(RayCastRender(startPoint_, endPoint_, color_), mode_);
	}
}

void Debug::DrawRayCast(Camera* cam_){
	if(rayManager != nullptr){
		rayManager->Render(cam_);
	}
}

//Clear Permanent rays on scene change
void Debug::OnSceneChange(){
	if(rayManager != nullptr){
		rayManager->ClearPermanent();
	}
}

//RayManager must be destroyed in the RenderEngine
//If we attempt to destroy it in Debug::Destroy it crashes because the ResourceManager no longer exists
void Debug::DestroyRays(){
	if(rayManager != nullptr){
		rayManager->Destroy();
	}
}

std::string Debug::GetFileName(const std::string& path_){
	char sep = '/';

#ifdef _WIN32
	sep = '\\';
#endif

	size_t i = path_.rfind(sep, path_.length());
	if(i != std::string::npos){
		return(path_.substr(i + 1, path_.length() - i));
	}
	return("");
}

void __stdcall Debug::GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
	//Suppress useless warnings by ID here
	//Fallthroughs here are deliberate
	switch(id){
		case 131169: //The driver allocated multisample storage for renderbuffer - Treated as low severity warning, should be info
		case 131185: //Buffer object will use VIDEO memory - Irrelevant and obvious, probably legacy
		case 131218: //Vertex shader is being recompiled based on GL state - Poorly defined NVIDIA thing, useless
		case 131204: //Texture object does not have a defined base level - Nobody seems to have any idea what this means or how to fix it
			return;
		default:
			break;
	}
	
	std::string finalMessage;
	finalMessage.append(std::to_string(id) + " OpenGL ");
	
	switch(source){
		case GL_DEBUG_SOURCE_API:
			finalMessage.append("API ");
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			finalMessage.append("Window System ");
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			finalMessage.append("Third Party ");
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			finalMessage.append("Application ");
			break;
		case GL_DEBUG_SOURCE_OTHER:
			break;
		default:
			break;
	}

	switch(type){
		case GL_DEBUG_TYPE_ERROR:
			finalMessage.append("Error: ");
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			finalMessage.append("Deprecated Behaviour: ");
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			finalMessage.append("Undefined Behaviour: ");
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			finalMessage.append("Portability: ");
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			finalMessage.append("Performance: ");
			break;
		case GL_DEBUG_TYPE_MARKER:
			finalMessage.append("Marker: ");
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			finalMessage.append("Push Group: ");
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			finalMessage.append("Push Group: ");
			break;
		case GL_DEBUG_TYPE_OTHER:
			//Intentional Fallthrough
		default:
			finalMessage.append("Other: ");
			break;
	}

	finalMessage.append(message);
	finalMessage.append("!");

	switch(severity){
		case GL_DEBUG_SEVERITY_HIGH:
			Debug::LogError(finalMessage);
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			Debug::LogWarning(finalMessage);
			break;
		case GL_DEBUG_SEVERITY_LOW:
			Debug::LogWarning(finalMessage);
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			Debug::Log(finalMessage);
			break;
	}
}