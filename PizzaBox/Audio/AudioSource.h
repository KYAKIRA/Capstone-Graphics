#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include <string>
#include <vector>

#include <fmod.hpp>

#include "AudioResource.h"
#include "Object/Component.h"

namespace PizzaBox{
	class AudioSource : public Component{
	public:
		enum class SoundType{
			_2D,
			_3D
		};

		AudioSource(const std::string& file_, const SoundType type_ = SoundType::_3D, const std::string& channel_ = "");
		~AudioSource();

		bool Initialize(GameObject* go_) override;
		void Destroy() override;

		void Update();
		void PlayOnce();
		void StopOnce();
		void PlayContinuous();
		void StopContinuous();
		bool IsPlaying() const;

	private:
		std::string resourceName;
		SoundType soundType;
		AudioResource* sound;
		std::vector<FMOD::Channel*> playingChannels;
		std::vector<FMOD::Channel*> loopingChannels;
		std::string volumeChannel;

		FMOD::Channel* Play(FMOD_MODE mode_);
		void StopAll();
	};
}

#endif //!AUDIO_SOURCE_H