#include "AudioSource.h"

#include <rttr/registration.h>
#include <fmod_errors.h>

#include "AudioManager.h"
#include "Object/GameObject.h"
#include "Physics/Rigidbody.h"
#include "Resource/ResourceManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<AudioSource>("AudioSource")
		.enumeration<AudioSource::SoundType>("SoundType")
		.constructor<const std::string&, const AudioSource::SoundType, const std::string&>()
		.method("Initialize", &AudioSource::Initialize)
		.method("Destroy", &AudioSource::Destroy)
		.method("Update", &AudioSource::Update)
		.method("PlayOnce", &AudioSource::PlayOnce)
		.method("StopOnce", &AudioSource::StopOnce)
		.method("PlayContinuous", &AudioSource::PlayContinuous)
		.method("StopContinuous", &AudioSource::StopContinuous)
		.method("IsPlaying", &AudioSource::IsPlaying);
}
#pragma warning( pop )

AudioSource::AudioSource(const std::string& file_, const SoundType type_, const std::string& channel_) : Component(), resourceName(file_), soundType(type_), sound(nullptr), volumeChannel(channel_){
	//Make sure an actual file path is being passed in and not just an empty string
	_ASSERT(!resourceName.empty());
}

AudioSource::~AudioSource(){
}

bool AudioSource::Initialize(GameObject* go_){
	//Make sure the GameObject pointer is valid
	_ASSERT(go_ != nullptr);

	gameObject = go_;

	sound = ResourceManager::LoadResource<AudioResource>(resourceName);
	if(sound == nullptr){
		Debug::LogError("Could not load sound file!", __FILE__, __LINE__);
		return false;
	}

	AudioManager::RegisterSource(this);

	return true;
}

void AudioSource::Destroy(){
	AudioManager::UnregisterSource(this);

	//Stop any playing or looping channels
	StopAll();

	if(sound != nullptr){
		ResourceManager::UnloadResource(resourceName);
		sound = nullptr;
	}

	gameObject = nullptr;
}

void AudioSource::Update(){
	for(FMOD::Channel* channel : playingChannels){
		channel->setVolume(AudioManager::MasterVolume() * AudioManager::GetVolume(volumeChannel));
	}

	for(FMOD::Channel* channel : loopingChannels){
		channel->setVolume(AudioManager::MasterVolume() * AudioManager::GetVolume(volumeChannel));
	}

	if(soundType == SoundType::_2D){
		//2D sounds don't use positional information, so we don't need to do anything here if the sound is 2D
		return;
	}

	FMOD_VECTOR _pos = gameObject->GetTransform()->GlobalPosition().ToFMOD(); //Position in 3D space
	FMOD_VECTOR _vel;
	//If this object has a Rigidbody, use the Rigidbody's velocity
	if(gameObject->GetComponent<Rigidbody>() != nullptr){
		_vel = gameObject->GetComponent<Rigidbody>()->GetLinearVelocity().ToFMOD();
	//Otherwise just use a zero vector
	}else{
		_vel = Vector3(0.0f, 0.0f, 0.0f).ToFMOD();
	}

	for(FMOD::Channel* channel : loopingChannels){
		FMOD_RESULT result = channel->set3DAttributes(&_pos, &_vel);
		if(result != FMOD_OK){
			Debug::LogError("Channel attributes could not be set! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		}
	}

	for(FMOD::Channel* channel : playingChannels){
		FMOD_RESULT result = channel->set3DAttributes(&_pos, &_vel);
		if(result != FMOD_OK){
			Debug::LogError("Channel attributes could not be set! FMOD Error: " + std::string(FMOD_ErrorString(result)), __FILE__, __LINE__);
		}
	}
}

void AudioSource::PlayOnce(){
	FMOD::Channel* channel = Play(FMOD_LOOP_OFF);

	if(channel != nullptr){
		playingChannels.push_back(channel);
	}
}

void AudioSource::StopOnce(){
	for(FMOD::Channel* channel : playingChannels){
		//FMOD keeps track of channels on its own, so we don't need to worry about deleting this
		channel->stop();
		channel = nullptr;
	}

	playingChannels.clear();
	playingChannels.shrink_to_fit();
}

void AudioSource::PlayContinuous(){
	FMOD::Channel* channel = Play(FMOD_LOOP_NORMAL);

	if(channel != nullptr){
		loopingChannels.push_back(channel);
	}
}

void AudioSource::StopContinuous(){
	for(FMOD::Channel* channel : loopingChannels){
		//FMOD keeps track of channels on its own, so we don't need to worry about deleting this
		channel->stop();
		channel = nullptr;
	}

	loopingChannels.clear();
	loopingChannels.shrink_to_fit();
}

bool AudioSource::IsPlaying() const{
	return (!playingChannels.empty()) || (!loopingChannels.empty());
}

//Function to hold the shared functionality of PlayOnce and PlayContinuous
FMOD::Channel* AudioSource::Play(FMOD_MODE mode){
	FMOD::Channel* channel = nullptr;

	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 velocity = Vector3(0.0f, 0.0f, 0.0f);
	if(mode == FMOD_3D){
		position = gameObject->GetTransform()->GlobalPosition();
		if(gameObject->GetComponent<Rigidbody>()){
			velocity = gameObject->GetComponent<Rigidbody>()->GetLinearVelocity();
		}
	}

	if(AudioManager::Play(sound, soundType, mode, &channel, position, velocity) == false){
		Debug::LogError("Sound could not be played!");
		return nullptr;
	}

	return channel;
}

void AudioSource::StopAll(){
	StopOnce();
	StopContinuous();
}