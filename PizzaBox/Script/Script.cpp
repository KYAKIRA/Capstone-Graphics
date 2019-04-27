#include "Script.h"

#include <rttr/registration.h>

#include "ScriptManager.h"
#include "Object/GameObject.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::enumeration<UIEvent>("UIEvent")(
		rttr::value("OnClick", UIEvent::OnClick),
		rttr::value("OnRelease", UIEvent::OnRelease)
	);

	rttr::registration::class_<CollisionInfo>("CollisionInfo")
		.constructor<GameObject*, const Vector3&>()
		.property("other", &CollisionInfo::other)
		.property("normal", &CollisionInfo::normal);

	rttr::registration::class_<Script>("Script")
		.method("Initialize", &Script::Initialize)
		.method("Destroy", &Script::Destroy)
		.method("HasStarted", &Script::HasStarted)
		.method("OnStart", &Script::OnStart)
		.method("Update", &Script::Update)
		.method("OnDestroy", &Script::OnDestroy)
		.method("OnCollision", &Script::OnCollision)
		.method("OnCollisionStay", &Script::OnCollisionStay)
		.method("OnCollisionExit", &Script::OnCollisionExit)
		.method("OnUI", &Script::OnUI)
		.method("RegisterCollisionEnter", &Script::RegisterCollisionEnter)
		.method("RegisterCollisionStay", &Script::RegisterCollisionStay)
		.method("RegisterCollisionExit", &Script::RegisterCollisionExit);
}
#pragma warning( pop )

Script::Script() : Component(), hasStarted(false){
}

Script::~Script(){
}

bool Script::Initialize(GameObject* go_){
	_ASSERT(go_ != nullptr);
	gameObject = go_;
	ScriptManager::RegisterScript(this);
	return true;
}

void Script::Destroy(){
	OnDestroy();
	ScriptManager::UnregisterScript(this);
}

bool Script::HasStarted(){
	return hasStarted;
}

void Script::OnStart(){
	hasStarted = true;
}

void Script::OnCollision(const CollisionInfo& info_){
}

void Script::OnCollisionStay(const CollisionInfo& info_){
}

void Script::OnCollisionExit(GameObject* other_){
}

void Script::OnUI(UIEvent event_){
}

void Script::RegisterCollisionEnter(CollisionInfo& info_){
	collisionEnterQueue.push(info_);
}

void Script::RegisterCollisionStay(CollisionInfo& info_){
	collisionStayQueue.push(info_);
}

void Script::RegisterCollisionExit(GameObject* other_){
	_ASSERT(other_ != nullptr);
	collisionExitQueue.push(other_);
}