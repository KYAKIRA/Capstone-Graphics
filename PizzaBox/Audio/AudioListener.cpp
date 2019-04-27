#include "AudioListener.h"

#include "AudioManager.h"
#include "Object/GameObject.h"
#include "Physics/Rigidbody.h"
#include "Math/Vector.h"

using namespace PizzaBox;

//Initialize the base component and any member variables
AudioListener::AudioListener() : Component(){
}

AudioListener::~AudioListener(){
}

bool AudioListener::Initialize(GameObject* go_){
	//GameObject must be a valid pointer
	_ASSERT(go_ != nullptr);

	gameObject = go_;
	AudioManager::RegisterListener(this);
	return true;
}

void AudioListener::Destroy(){
	AudioManager::UnregisterListener(this);
	//If we're destroying a component then it's no longer attached to a GameObject
	gameObject = nullptr;
}

FMOD_VECTOR AudioListener::Position() const{
	//GameObject must have a transform
	_ASSERT(gameObject->GetTransform() != nullptr);

	//Convert the global position vector to an FMOD vector so that FMOD can use it
	return gameObject->GetTransform()->GlobalPosition().ToFMOD();
}

FMOD_VECTOR AudioListener::Velocity() const{
	//If the GameObject has a Rigidbody component, convert it to an FMOD vector so that FMOD can use it
	//Otherwise, just create an empty vector to pass to it
	if(gameObject->GetComponent<Rigidbody>() != nullptr){
		return gameObject->GetComponent<Rigidbody>()->GetLinearVelocity().ToFMOD();
	}else{
		return Vector3::Zero().ToFMOD();
	}
}

FMOD_VECTOR AudioListener::Forward() const{
	//GameObject must have a transform
	_ASSERT(gameObject->GetTransform() != nullptr);

	//Convert the forward vector to an FMOD vector so that FMOD can use it
	return (-gameObject->GetTransform()->GetForward()).ToFMOD();
}

FMOD_VECTOR AudioListener::Up() const{
	//Make sure this GameObject has a transform
	_ASSERT(gameObject->GetTransform() != nullptr);

	//Convert the up vector to an FMOD vector so that FMOD can use it
	return gameObject->GetTransform()->GetUp().ToFMOD();
}