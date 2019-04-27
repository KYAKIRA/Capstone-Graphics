#ifndef AUDIO_RESOURCE_H
#define AUDIO_RESOURCE_H

#include <fmod.hpp>

#include "../Resource/Resource.h"

namespace PizzaBox{
	class AudioResource : public Resource{
	public:
		AudioResource(const std::string file_);
		~AudioResource();

		bool Load() override;
		void Unload() override;

		FMOD::Sound* Sound2D();
		FMOD::Sound* Sound3D();
	private:
		FMOD::Sound* sound2D;
		FMOD::Sound* sound3D;
	};
}

#endif //!AUDIO_RESOURCE_H