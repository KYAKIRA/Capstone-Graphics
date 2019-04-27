#include "Time.h"

#include <rttr/registration.h>
#include <SDL.h>

#include "Config.h"

using namespace PizzaBox;

//Initialize static variables here
unsigned int Time::frameRate = 0;
float Time::timeScale = 1.0f;
unsigned int Time::startTime = 0;
unsigned int Time::prevTicks = 0;
unsigned int Time::currTicks = 0;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Time>("Time")
		.method("Initialize", &Time::Initialize)
		.method("Destroy", &Time::Destroy)
		.method("Start", &Time::Start)
		.method("DeltaTime", &Time::DeltaTime)
		.method("UpdateFrameTicks", &Time::UpdateFrameTicks)
		.method("RealDeltaTime", &Time::RealDeltaTime)
		.method("PureDeltaTime", &Time::PureDeltaTime)
		.method("Delay", &Time::PureDeltaTime)
		.method("SetFrameTime", &Time::SetFrameRate)
		.method("SetTimeScale", &Time::SetTimeScale)
		.method("TimeSinceStartup", &Time::TimeSinceStartup);
}
#pragma warning( pop )

bool Time::Initialize(){
	frameRate = Config::GetInt("TargetFPS");
	return true;
}

void Time::Destroy(){
	timeScale = 1.0f;
	startTime = 0;
	prevTicks = 0;
	currTicks = 0; 
}

void Time::Start(){
	startTime = SDL_GetTicks();
	prevTicks = startTime;
	currTicks = startTime;
}

float Time::DeltaTime(){
	return RealDeltaTime() * timeScale;
}

void Time::UpdateFrameTicks(){
	prevTicks = currTicks;
	currTicks = SDL_GetTicks();
}

float Time::RealDeltaTime(){
	float deltaTime = PureDeltaTime();
	
	//If the actual change in time is more than a tenth of a second, then just use 1/10 as the deltaTime
	//This is to prevent time-related glitches in the event of hitting a breakpoint, a window event, a lag spike, etc
	//The actual value here is somewhat arbitrary, but to maximize its effect it should be somewhere between 1/5 and 1/20
	//If it's too low it'll affect normal framerates, and if it's too high it won't be effective enough
	if(deltaTime > 1.0f / 10.0f){
		deltaTime = 1.0f / 10.0f;
	}

	return deltaTime;
}

float Time::PureDeltaTime(){
	return static_cast<float>(currTicks - prevTicks) / 1000.0f;
}

void Time::Delay(){
	const unsigned int sleepTime = GetSleepTime();

	if(sleepTime > 0){
		SDL_Delay(sleepTime);	
	}
}

void Time::SetFrameRate(const unsigned int target){
	frameRate = target;
}

void Time::SetTimeScale(const float scale){
	timeScale = scale;
}

double Time::TimeSinceStartup(){
	return static_cast<double>(SDL_GetTicks() - startTime) / 1000.0;
}

unsigned int Time::GetSleepTime(){
	//If the framerate is 0, this is treated as an unlimited framerate
	//This also prevents a division by 0 on the next line
	if(frameRate == 0){
		return 0;
	}

	unsigned int msPerFrame = 1000 / frameRate;

	if(msPerFrame == 0){
		return 0;
	}

	unsigned int sleepTime = msPerFrame - (SDL_GetTicks() - currTicks);
	if(sleepTime > msPerFrame){
		return msPerFrame;
	}

	return sleepTime;
}