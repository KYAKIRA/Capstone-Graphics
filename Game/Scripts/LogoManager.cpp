#include "LogoManager.h"

#include <Input/InputManager.h>
#include <Core/GameManager.h>
#include <Core/Time.h>
#include <Core/SceneManager.h>

using namespace GamePackage;

LogoManager::LogoManager(PizzaBox::ImageUI* image_, float endTime_, float waitTime_) : image(image_), currentTime(0.0f), transparency(0.0f), endTime(endTime_), waitTime(waitTime_){
	_ASSERT(image != nullptr);
}

void LogoManager::OnStart(){
	_ASSERT(image != nullptr);

	Script::OnStart();
	currentTime = 0.0f;
}

void LogoManager::Update(const float deltaTime_){
	currentTime += PizzaBox::Time::RealDeltaTime();

	//Early escape
	if(PizzaBox::InputManager::GetButtonDown("Exit")){
		PizzaBox::GameManager::Stop();
		return;
	}

	if(currentTime > endTime){
		PizzaBox::SceneManager::LoadScene(1);
		return;
	}

	if(currentTime >= 0.0f && currentTime < (endTime / 2.0f - waitTime)){
		//Transparency building up to half - waitTime
		transparency = currentTime / (endTime / 2.0f - waitTime);
	}else if(currentTime >= (endTime / 2.0f - waitTime) && currentTime < (endTime / 2.0f)){
		//Transparency holding for wait time
		transparency = 1.0f;
	}else{
		//Transparency reducing after half 
		transparency = ((endTime - currentTime) / (endTime / 2.0f));
	}

	image->SetTransparency(transparency);
}

void LogoManager::OnDestroy(){
	image = nullptr; //We don't delete the image because we don't own it
}