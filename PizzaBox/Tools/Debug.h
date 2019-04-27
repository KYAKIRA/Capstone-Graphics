#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <map>

#include "Profiler.h" 
#include "Graphics/Color.h"
#include "Math/Vector.h"
 
//Forward declarations
typedef void* HANDLE;
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef char GLchar;

namespace PizzaBox{
	//More forward declarations
	class Camera;
	class RayManager;

	//How many times the Ray gets set/how many times Debug::DrawRayCast() gets called
	enum class RayMode{
		Permanent, //Only get called once such as scene init
		Repeat //Gets called in an update or render
	};

	class Debug{
	public:
		static bool Initialize();
		static void Destroy();

		static void Log(const std::string& log, const std::string& file, int line);
		static void Log(const std::string& log);

		static void LogWarning(const std::string& warning, const std::string& file, int line);
		static void LogWarning(const std::string& warning);

		static void LogError(const std::string& error, const std::string& file, int line);
		static void LogError(const std::string& error);

		static void DisplayFatalErrorMessage(const std::string& title, const std::string& message_);

		static void StartProfiling(const std::string& profilerName_);
		static double EndProfiling(const std::string& profilerName_);
		static Profiler* GetProfiler(const std::string& profiler_);

		static bool InitializeRays();
		static void CastRay(const Vector3& startPoint_, const Vector3& direction_, RayMode mode_, float length_ = 20.0f, const Color& color_ = Color::White);
		static void CastRayPoints(const Vector3& startPoint_, const Vector3& endPoint_, RayMode mode, const Color& color_ = Color::White);
		static void DrawRayCast(Camera* cam);
		static void OnSceneChange();
		static void DestroyRays();

		static void __stdcall GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

		//Delete unwanted compiler generated constructors, destructor and assignment operators
		Debug() = delete;
		Debug(const Debug&) = delete;
		Debug(Debug&&) = delete;
		Debug& operator=(const Debug&) = delete;
		Debug& operator=(Debug&&) = delete;
		~Debug() = delete;

	private:
		static const HANDLE hConsole;
		static std::map<const std::string, Profiler*> profilers;
		static RayManager* rayManager;

		static void WriteLogFile(const std::string& message, const std::string& file, int line);
		static void WriteLogFile(const std::string& message);
		static std::string GetFileName(const std::string& s); 
	};
}

#endif //!DEBUG_H