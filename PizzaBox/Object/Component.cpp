#include "Component.h"

#include <rttr/registration.h>

#include "GameObject.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Component>("Component")
		.method("Initialize", &Component::Initialize)
		.method("Destroy", &Component::Destroy)
		.method("GetEnable", &Component::GetEnable)
		.method("GetGameObject", &Component::GetGameObject)
		.method("SetEnable", &Component::SetEnable);
}
#pragma warning( pop )