#include "AudioResource.h"

#include <fmod_errors.h>

#include "AudioManager.h"
#include "../Tools/Debug.h"

using namespace PizzaBox;

AudioResource::AudioResource(const std::string file_) : Resource(file_), sound2D(nullptr), sound3D(nullptr){
}

AudioResource::~AudioResource(){
}

bool AudioResource::Load(){
	if(AudioManager::CreateSound(fileName, &sound2D, &sound3D) == false){
		Debug::LogError("Sound could not be created!");
		return false;
	}

	return true;
}

void AudioResource::Unload(){
	FMOD_RESULT result;

	//Unload tht 2D sound
	if(sound2D != nullptr){
		result = sound2D->release();
		if(result != FMOD_OK){
			Debug::LogError("Sound was not properly released! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		}

		sound2D = nullptr;
	}

	//Unload the 3D sound
	if(sound3D != nullptr){
		result = sound3D->release();
		if(result != FMOD_OK){
			Debug::LogError("Sound was not properly released! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		}

		sound3D = nullptr;
	}
}

FMOD::Sound* AudioResource::Sound2D(){
	return sound2D;
}

FMOD::Sound* AudioResource::Sound3D(){
	return sound3D;
}