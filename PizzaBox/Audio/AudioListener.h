#ifndef AUDIO_LISTENER_H
#define AUDIO_LISTENER_H

#include <fmod.hpp>

#include "Object/Component.h"

namespace PizzaBox{
	class AudioListener : public Component{
	public:
		AudioListener();
		virtual ~AudioListener() override;

		virtual bool Initialize(GameObject* go_) override;
		virtual void Destroy() override;

		FMOD_VECTOR Position() const;
		FMOD_VECTOR Velocity() const;
		FMOD_VECTOR Forward() const;
		FMOD_VECTOR Up() const;
	};
}

#endif //!AUDIO_LISTENER_H