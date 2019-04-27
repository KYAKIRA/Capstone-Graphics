#include "AudioManager.h"

#include <fmod_errors.h>

#include "Core/Config.h"
#include "Core/SceneManager.h"
#include "Math/Math.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Initialize static variables
FMOD::Studio::System* AudioManager::studioSystem = nullptr;
FMOD::System* AudioManager::system = nullptr;
std::vector<AudioListener*> AudioManager::listeners;
std::vector<AudioSource*> AudioManager::sources;
float AudioManager::masterVolume = 1.0f;
std::map<std::string, float> AudioManager::channelVolumes;

bool AudioManager::Initialize(){
	//Create the FMOD Studio System
	FMOD_RESULT result = FMOD::Studio::System::create(&studioSystem);
	if(result != FMOD_OK){
		Debug::LogError("FMOD Studio System could not be created! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		return false;
	}

	//Get the Low Level System from the Studio System
	result = studioSystem->getLowLevelSystem(&system);
	if(result != FMOD_OK){
		Debug::LogError("Could not get the Low Level System! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		return false;
	}

	//Get the version of FMOD from the library
	unsigned int version;
	result = system->getVersion(&version);
	if(result != FMOD_OK){
		Debug::LogError("FMOD Studio System version could not be found! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		return false;
	}

	//Check the header version number against the library version number
	//If they don't match, then the library is incompatible with the headers
	if(version != FMOD_VERSION){
		Debug::LogError("FMOD library version (" + std::to_string(version) + ") does not match header version (" + std::to_string(FMOD_VERSION) + ")!", __FILE__, __LINE__);
		return false;
	}

	//You can use this to customize advanced settings
	//system->setAdvancedSettings(FMOD_STUDIO_ADVANCEDSETTINGS);

	int maxChannels = Config::GetInt("MaxAudioChannels");
	if(maxChannels > 4095){
		Debug::LogWarning("FMOD cannot handle more than 4095 max channels, attempted to use [" + std::to_string(maxChannels) + "!", __FILE__, __LINE__);
		maxChannels = 4095;
	}else if(maxChannels < 0){
		Debug::LogError("Max number of audio channels must be a positive value!", __FILE__, __LINE__);
		return false;
	}

	//Initialize the FMOD studio system (which also initialized the low level system)
	result = studioSystem->initialize(maxChannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
	if(result != FMOD_OK){
		Debug::LogError("FMOD Studio System could not be initialized! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		return false;
	}

	//Set the maximum number of audible voices
	//system->setSoftwareChannels(64);

	masterVolume = Config::GetFloat("MasterVolume");

	return true;
}

void AudioManager::Destroy(){
	if(studioSystem != nullptr){
		FMOD_RESULT result = studioSystem->release();
		if(result != FMOD_OK){
			Debug::LogError("FMOD Studio System was not released! FMOD Error Code: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		}

		studioSystem = nullptr;
		system = nullptr;
	}
}

void AudioManager::RegisterListener(AudioListener* listener_){
	_ASSERT(listener_ != nullptr);
	listeners.push_back(listener_);
}

void AudioManager::RegisterSource(AudioSource* source_){
	_ASSERT(source_ != nullptr);
	sources.push_back(source_);
}

void AudioManager::UnregisterListener(AudioListener* listener_){
	_ASSERT(listener_ != nullptr);
	listeners.erase(std::remove(listeners.begin(), listeners.end(), listener_), listeners.end());
}

void AudioManager::UnregisterSource(AudioSource* source_){
	_ASSERT(source_ != nullptr);
	sources.erase(std::remove(sources.begin(), sources.end(), source_), sources.end());
}

void AudioManager::Update(){
	//Audio Listener(s) need to be updated every frame
	for(size_t i = 0; i < listeners.size(); i++){
		//Don't do anything to this one if it's not enabled
		if(!listeners[i]->GetEnable()){
			continue;
		}

		FMOD_VECTOR pos = listeners[i]->Position();
		FMOD_VECTOR vel = listeners[i]->Velocity();
		FMOD_VECTOR forw = listeners[i]->Forward();
		FMOD_VECTOR up = listeners[i]->Up();
		system->set3DListenerAttributes(static_cast<int>(i), &pos, &vel, &forw, &up);
	}

	//Audio Sources that are looping also need to be updated every frame
	for(AudioSource* aS : sources){
		//Don't do anything to this one if it's not enabled
		if(!aS->GetEnable()){
			continue;
		}

		aS->Update();
	}

	//Update FMOD itself
	studioSystem->update();
	system->update(); //Not sure if this is needed
}

bool AudioManager::CreateSound(const std::string& filePath_, FMOD::Sound** sound2D_, FMOD::Sound** sound3D_){
	//Load the sound as a 2D sound
	FMOD_RESULT result = system->createSound(filePath_.c_str(), FMOD_2D, 0, sound2D_);
	if(result != FMOD_OK){
		Debug::LogError("2D Sound could not be created! FMOD Error Code: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		return false;
	}

	//Load the sound as a 3D sound
	result = system->createSound(filePath_.c_str(), FMOD_3D, 0, sound3D_);
	if(result != FMOD_OK){
		Debug::LogError("3D Sound could not be created! FMOD Error Code: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		return false;
	}

	return true;
}

bool AudioManager::Play(AudioResource* sound_, AudioSource::SoundType soundType_, FMOD_MODE mode_, FMOD::Channel** channel_, const Vector3& position_, const Vector3& velocity_){
	FMOD_RESULT result = FMOD_OK;
	
	//Start the sound paused so we can change it's attributes before the user hears anything
	//If we don't do this the audio may 'pop' when we change the attributes
	if(soundType_ == AudioSource::SoundType::_2D){
		result = system->playSound(sound_->Sound2D(), 0, true, channel_);
	}else if(soundType_ == AudioSource::SoundType::_3D){
		result = system->playSound(sound_->Sound3D(), 0, true, channel_);
	}

	if(result != FMOD_OK){
		Debug::LogError("Sound could not be played! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		return false;
	}

	//If it's 3D audio then we need to worry about position and velocity information
	if(soundType_ == AudioSource::SoundType::_3D){
		const FMOD_VECTOR _pos = position_.ToFMOD();	//Position in 3D space
		const FMOD_VECTOR _vel = velocity_.ToFMOD();	//Velocity

		result = (*channel_)->set3DAttributes(&_pos, &_vel);
		if(result != FMOD_OK){
			Debug::LogError("Channel attributes could not be set! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
			return false;
		}
	}

	//Set whether the channel is looping or not
	result = (*channel_)->setMode(mode_);
	if(result != FMOD_OK){
		Debug::LogError("Channel mode could not be set! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		return false;
	}

	//Unpause the sound to play it
	result = (*channel_)->setPaused(false);
	if(result != FMOD_OK){
		Debug::LogError("Channel could not be unpaused! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		return false;
	}

	return true;
}

float AudioManager::MasterVolume(){
	return masterVolume;
}

float AudioManager::GetVolume(const std::string& channel_){
	//This allows us to have audio sources with no channel set
	if(channel_.empty()){
		return 1.0f;
	}

	_ASSERT(channelVolumes.find(channel_) != channelVolumes.end());
	return channelVolumes[channel_];
}

void AudioManager::SetMasterVolume(float volume_){
	masterVolume = Math::Clamp(0.0f, 1.0f, volume_);
	Config::SetFloat("MasterVolume", masterVolume);
}

void AudioManager::SetVolume(const std::string& channel_, float volume_){
	_ASSERT(channelVolumes.find(channel_) != channelVolumes.end());
	channelVolumes[channel_] = Math::Clamp(0.0f, 1.0f, volume_);
	Config::SetFloat(channel_ + "Volume", channelVolumes[channel_]);
}

void AudioManager::CreateVolumeChannel(const std::string& channel_){
	_ASSERT(channelVolumes.find(channel_) == channelVolumes.end());
	channelVolumes.insert(std::pair<std::string, float>(channel_, Config::GetFloat(channel_ + "Volume")));
}