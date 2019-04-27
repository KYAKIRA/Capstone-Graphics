#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <map>
#include <vector>

#include <fmod_studio.hpp>

#include "AudioListener.h"
#include "AudioResource.h"
#include "AudioSource.h"
#include "Math/Vector.h"

namespace PizzaBox{
	class AudioManager{
	public:
		static bool Initialize();
		static void Destroy();

		static void RegisterListener(AudioListener* listener_);
		static void RegisterSource(AudioSource* source_);

		static void UnregisterListener(AudioListener* listener_);
		static void UnregisterSource(AudioSource* source_);

		static void Update();

		static bool CreateSound(const std::string& filePath_, FMOD::Sound** sound2D_, FMOD::Sound** sound3D_);
		static bool Play(AudioResource* sound_, AudioSource::SoundType soundType_, FMOD_MODE mode_, FMOD::Channel** channel_, const Vector3& position_ = Vector3(0.0f, 0.0f, 0.0f), const Vector3& velocity_ = Vector3(0.0f, 0.0f, 0.0f));
		
		static float MasterVolume();
		static float GetVolume(const std::string& channel_);
		static void SetMasterVolume(float volume_);
		static void SetVolume(const std::string& channel_, float volume_);

		static void CreateVolumeChannel(const std::string& type_);

		//Get rid of unwanted compiler-generated constructors, assignment operators and destructors
		AudioManager() = delete;
		AudioManager(const AudioManager&) = delete;
		AudioManager(AudioManager&&) = delete;
		AudioManager& operator=(const AudioManager&) = delete;
		AudioManager& operator=(AudioManager&&) = delete;
		~AudioManager() = delete;

	private:
		static FMOD::Studio::System* studioSystem;
		static FMOD::System* system;

		static std::vector<AudioListener*> listeners;
		static std::vector<AudioSource*> sources;
		static float masterVolume;
		static std::map<std::string, float> channelVolumes;
	};
}

#endif //!AUDIO_MANAGER_H