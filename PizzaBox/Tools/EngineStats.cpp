#include "EngineStats.h"

#include <rttr/registration.h>

#include "Debug.h"
#include "../Math/Math.h"

using namespace PizzaBox;

float EngineStats::updateTimer = 0.0f;
float EngineStats::fpsCurrent = 0.0f;
int EngineStats::frameCount = 0;
std::map<std::string, float> EngineStats::floatStats;
std::map<std::string, long long> EngineStats::intStats;
std::map<std::string, std::string> EngineStats::stringStats;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<EngineStats>("EngineStats")
		.method("GetFloat", &EngineStats::GetFloat)
		.method("GetInt", &EngineStats::GetInt)
		.method("GetString", &EngineStats::GetString)
		.method("AddToFloat", &EngineStats::AddToFloat)
		.method("AddToInt", &EngineStats::AddToInt)
		.method("SetFloat", &EngineStats::SetFloat)
		.method("SetInt", &EngineStats::SetInt)
		.method("SetString", &EngineStats::SetString);
}
#pragma warning( pop )

bool EngineStats::Initialize(){
	_ASSERT(floatStats.size() == 0);
	_ASSERT(intStats.size() == 0);
	_ASSERT(stringStats.size() == 0);

	SetFloat("Current FPS", 0.0f);
	SetFloat("Average FPS", 0.0f);
	SetFloat("Current Frame Time", 0.0f);
	SetFloat("Average Frame Time", 0.0f);
	SetInt("Total Frame Count", 0);

	return true;
}

void EngineStats::Destroy(){
	floatStats.clear();
	intStats.clear();
	stringStats.clear();
	updateTimer = 0.0f;
	fpsCurrent = 0.0f;
	frameCount = 0;
}

void EngineStats::Update(float deltaTime_){
	AddToInt("Total Frame Count", 1);

	if(Debug::GetProfiler("Main Game Loop") != nullptr){
		SetFloat("Current Frame Time", static_cast<float>(Debug::GetProfiler("Main Game Loop")->GetNewestValue()));
		SetFloat("Average Frame Time", static_cast<float>(Debug::GetProfiler("Main Game Loop")->GetAverage()));
	}

	if(!Math::NearZero(deltaTime_)){
		SetFloat("Current FPS", 1.0f / deltaTime_);
	}else{
		SetFloat("Current FPS", 0.0f);
	}

	updateTimer += deltaTime_;
	if(updateTimer >= 1.0f){
		if(Math::NearZero(fpsCurrent) || frameCount <= 0){
			SetFloat("Average FPS", 0.0f);
		}else{
			SetFloat("Average FPS", fpsCurrent / frameCount);
		}
		
		fpsCurrent = 0.0f;
		frameCount = 0;
		updateTimer -= 1.0f;
	}else if(!Math::NearZero(deltaTime_)){
		fpsCurrent += 1.0f / deltaTime_;
		frameCount++;
	}
}

float EngineStats::GetFloat(const std::string& key_){
	_ASSERT(floatStats.find(key_) != floatStats.end());
	return floatStats[key_];
}

long long EngineStats::GetInt(const std::string& key_){
	_ASSERT(intStats.find(key_) != intStats.end());
	return intStats[key_];
}

std::string EngineStats::GetString(const std::string& key_){
	_ASSERT(stringStats.find(key_) != stringStats.end());
	return stringStats[key_];
}

void EngineStats::AddToFloat(const std::string& key_, float add_){
	if(floatStats.find(key_) == floatStats.end()){
		floatStats.insert(std::make_pair(key_, add_));
	}else{
		floatStats[key_] += add_;
	}
}

void EngineStats::AddToInt(const std::string& key_, long long add_){
	if(intStats.find(key_) == intStats.end()){
		intStats.insert(std::make_pair(key_, add_));
	}else{
		intStats[key_] += add_;
	}
}

void EngineStats::SetFloat(const std::string& key_, float value_){
	if(floatStats.find(key_) == floatStats.end()){
		floatStats.insert(std::make_pair(key_, value_));
	}else{
		floatStats[key_] = value_;
	}
}

void EngineStats::SetInt(const std::string& key_, long long value_){
	if(intStats.find(key_) == intStats.end()){
		intStats.insert(std::make_pair(key_, value_));
	}else{
		intStats[key_] = value_;
	}
}

void EngineStats::SetString(const std::string& key_, const std::string& value_){
	if(stringStats.find(key_) == stringStats.end()){
		stringStats.insert(std::make_pair(key_, value_));
	}else{
		stringStats[key_] = value_;
	}
}