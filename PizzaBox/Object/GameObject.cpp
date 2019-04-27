#include "GameObject.h"

#include <rttr/registration.h>

#include "Core/GameManager.h"
#include "Core/SceneManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<GameObject>("GameObject")
		.constructor()
		.constructor<const Vector3&>()
		.constructor<const Vector3&, const Euler&>()
		.constructor<const Vector3&, const Euler&, const Vector3&>()
		.method("Initialize", &GameObject::Initialize)
		.method("Destroy", &GameObject::Destroy)
		.method("Update", &GameObject::Update)
		.method("AddChild", &GameObject::AddChild)
		.method("AddComponent", &GameObject::AddComponent)
		.method("RemoveChild", &GameObject::RemoveChild)
		.method("RemoveAllChildren", &GameObject::RemoveAllChildren)
		.method("AttachChild", &GameObject::AttachChild)
		.method("DetachChild", &GameObject::DetachChild)
		.method("GetTransform", static_cast<Transform*(GameObject::*)()>(&GameObject::GetTransform))
		.method("GetTransform", static_cast<const Transform*(GameObject::*)() const>(&GameObject::GetTransform))
		.method("GetChild", &GameObject::GetChild)
		.method("GetPosition", &GameObject::GetPosition)
		.method("GlobalPosition", &GameObject::GlobalPosition)
		.method("GetRotation", &GameObject::GetRotation)
		.method("GetRotationQuat", &GameObject::GetRotationQuat)
		.method("GlobalRotation", &GameObject::GlobalRotation)
		.method("GlobalRotationQuat", &GameObject::GlobalRotationQuat)
		.method("GetScale", &GameObject::GetScale)
		.method("GlobalScale", &GameObject::GlobalScale)
		.method("IsStatic", &GameObject::IsStatic)
		.method("GetForward", &GameObject::GetForward)
		.method("GetUp", &GameObject::GetUp)
		.method("GetRight", &GameObject::GetRight)
		.method("SetPosition", static_cast<void(GameObject::*)(const Vector3&)>(&GameObject::SetPosition))
		.method("SetPosition", static_cast<void(GameObject::*)(float, float, float)>(&GameObject::SetPosition))
		.method("SetRotation", static_cast<void(GameObject::*)(const Euler&)>(&GameObject::SetRotation))
		.method("SetRotation", static_cast<void(GameObject::*)(float, float, float)>(&GameObject::SetRotation))
		.method("SetRotation", static_cast<void(GameObject::*)(const Matrix3&)>(&GameObject::SetRotation))
		.method("SetRotation", static_cast<void(GameObject::*)(const Quaternion&)>(&GameObject::SetRotation))
		.method("SetScale", static_cast<void(GameObject::*)(const Vector3&)>(&GameObject::SetScale))
		.method("SetScale", static_cast<void(GameObject::*)(float, float, float)>(&GameObject::SetScale))
		.method("SetScale", static_cast<void(GameObject::*)(float)>(&GameObject::SetScale))
		.method("SetGlobalPosition", static_cast<void(GameObject::*)(const Vector3&)>(&GameObject::SetGlobalPosition))
		.method("SetGlobalPosition", static_cast<void(GameObject::*)(float, float, float)>(&GameObject::SetGlobalPosition))
		.method("SetGlobalRotation", static_cast<void(GameObject::*)(const Euler&)>(&GameObject::SetGlobalRotation))
		.method("SetGlobalRotation", static_cast<void(GameObject::*)(float, float, float)>(&GameObject::SetGlobalRotation))
		.method("SetGlobalRotation", static_cast<void(GameObject::*)(const Matrix3&)>(&GameObject::SetGlobalRotation))
		.method("SetGlobalRotation", static_cast<void(GameObject::*)(const Quaternion&)>(&GameObject::SetGlobalRotation))
		.method("SetGlobalScale", static_cast<void(GameObject::*)(const Vector3&)>(&GameObject::SetGlobalScale))
		.method("SetGlobalScale", static_cast<void(GameObject::*)(float, float, float)>(&GameObject::SetGlobalScale))
		.method("SetGlobalScale", static_cast<void(GameObject::*)(float)>(&GameObject::SetGlobalScale))
		.method("SetStatic", &GameObject::SetStatic)
		.method("Translate", static_cast<void(GameObject::*)(const Vector3&)>(&GameObject::Translate))
		.method("Translate", static_cast<void(GameObject::*)(float, float, float)>(&GameObject::Translate))
		.method("Rotate", static_cast<void(GameObject::*)(const Euler&)>(&GameObject::Rotate))
		.method("Rotate", static_cast<void(GameObject::*)(float, float, float)>(&GameObject::Rotate))
		.method("Rotate", static_cast<void(GameObject::*)(const Matrix3&)>(&GameObject::Rotate))
		.method("Rotate", static_cast<void(GameObject::*)(const Quaternion&)>(&GameObject::Rotate))
		.method("SetTag", &GameObject::SetTag)
		.method("RemoveTag", &GameObject::RemoveTag)
		.method("HasTag", &GameObject::HasTag);
		//TODO - Figure out how to handle template functions
}
#pragma warning( pop )

GameObject::GameObject(const Vector3& position_, const Euler& rotation_, const Vector3& scale_) : transform(Transform(position_, rotation_, scale_)), components(std::vector<Component*>()), children(std::vector<GameObject*>()), tags(std::set<std::string>()), hasInitialized(false), isStatic(false){
}

GameObject::~GameObject(){
	#ifdef _DEBUG
	if(!children.empty() || !components.empty() || !tags.empty() || !addComponentQueue.empty() || !removeComponentQueue.empty() || !createChildQueue.empty() || !removeChildQueue.empty()){
		Debug::LogWarning("Memory leak detected in GameObject!", __FILE__, __LINE__);
		Destroy();
	}
	#endif // _DEBUG
}

bool GameObject::Initialize(){
	Update();

	if(hasInitialized){
		Debug::LogWarning("You tried initializing an object that has already been initialized!", __FILE__, __LINE__);
		return true;
	}

	for(Component* component : components){
		if(component->Initialize(this) == false){
			Debug::LogError("Component could not be initialized!", __FILE__, __LINE__);
			return false;
		}
	}

	for(GameObject* go : children){
		if(go->Initialize() == false){
			Debug::LogError("Child object could not be initialized!", __FILE__, __LINE__);
			return false;
		}
	}

	hasInitialized = true;
	return true;
}

void GameObject::Destroy(){
	for(Component* component : components){
		if(component != nullptr){
			component->Destroy();
			delete component;
			component = nullptr;
		}
	}

	for(GameObject* child : children){
		if(child != nullptr){
			child->Destroy();
			delete child;
			child = nullptr;
		}
	}

	components.clear();
	components.shrink_to_fit();
	children.clear();
	children.shrink_to_fit();

	tags.clear();

	while(!addComponentQueue.empty()){
		addComponentQueue.front()->Destroy();
		delete addComponentQueue.front();
		addComponentQueue.pop();
	}

	while(!removeComponentQueue.empty()){
		//Everything in this queue has already been deleted by this point
		removeComponentQueue.pop();
	}

	while(!createChildQueue.empty()){
		createChildQueue.front()->Destroy();
		delete createChildQueue.front();
		createChildQueue.pop();
	}

	while(!removeChildQueue.empty()){
		//Everything in this queue has already been deleted by this point
		removeChildQueue.pop();
	}
}

bool GameObject::Update(){
	while(!addComponentQueue.empty()){
		Component* c = addComponentQueue.front();
		addComponentQueue.pop();

		components.push_back(c);

		if(hasInitialized && c->Initialize(this) == false){
			Debug::DisplayFatalErrorMessage("Component Error", "Component failed to initialize!");
			return false;
		}
	}

	while(!removeComponentQueue.empty()){
		Component** c = removeComponentQueue.front();
		removeComponentQueue.pop();

		if((*c) != nullptr){
			(*c)->Destroy();
			delete (*c);
			(*c) = nullptr;
		}
	}

	//Remove all nullptrs from the component list
	components.erase(std::remove(components.begin(), components.end(), nullptr), components.end());

	while(!createChildQueue.empty()){
		GameObject* go = createChildQueue.front();
		createChildQueue.pop();

		children.push_back(go);
		go->GetTransform()->SetInitialParent(&transform);

		if(hasInitialized && go->Initialize() == false){
			Debug::DisplayFatalErrorMessage("GameObject Error", "Child GameObject failed to initialize!");
			return false;
		}
	}

	while(!removeChildQueue.empty()){
		GameObject* go = removeChildQueue.front();
		removeChildQueue.pop();

		if(go != nullptr){
			go->Destroy();
			delete go;
		}

		children.erase(std::remove(children.begin(), children.end(), go), children.end());
		go = nullptr;
	}

	while(!attachChildQueue.empty()){
		GameObject* go = attachChildQueue.front();
		attachChildQueue.pop();
		SceneManager::CurrentScene()->DetachObject(go);
		children.push_back(go);
		go->GetTransform()->SetParent(&transform);
	}

	while(!detachChildQueue.empty()){
		GameObject* go = detachChildQueue.front();
		detachChildQueue.pop();
		SceneManager::CurrentScene()->AttachObject(go);
		go->GetTransform()->SetParent(nullptr);
		children.erase(std::remove(children.begin(), children.end(), go), children.end());
	}

	for(GameObject* child : children){
		if(child->hasInitialized){
			child->Update();
		}
	}

	return true;
}

void GameObject::AddChild(GameObject* child_){
	_ASSERT(child_ != nullptr);

	createChildQueue.push(child_);
}

void GameObject::AddComponent(Component* component_){
	_ASSERT(component_ != nullptr);

	addComponentQueue.push(component_);
}

void GameObject::RemoveChild(GameObject* child_) {
	_ASSERT(child_ != nullptr);
	removeChildQueue.push(child_);
}

void GameObject::RemoveAllChildren(){
	for(GameObject* go : children){
		removeChildQueue.push(go);
	}
}

void GameObject::AttachChild(GameObject* child_){
	attachChildQueue.push(child_);
}

void GameObject::DetachChild(GameObject* child_){
	detachChildQueue.push(child_);
}

Transform* GameObject::GetTransform(){
	return &transform;
}

const Transform* GameObject::GetTransform() const{
	return &transform;
}

GameObject* GameObject::GetChild(unsigned int index_) const{
	_ASSERT(index_ < children.size());

	return children[index_];
}

Vector3 GameObject::GetPosition() const{
	return transform.GetPosition();
}

Vector3 GameObject::GlobalPosition() const{
	return transform.GlobalPosition();
}

Euler GameObject::GetRotation() const{
	return transform.GetRotation();
}

Quaternion GameObject::GetRotationQuat() const{
	return transform.GetRotationQuat();
}

Euler GameObject::GlobalRotation() const{
	return transform.GlobalRotation();
}

Quaternion GameObject::GlobalRotationQuat() const{
	return transform.GlobalRotationQuat();
}

Vector3 GameObject::GetScale() const{
	return transform.GetScale();
}

Vector3 GameObject::GlobalScale() const{
	return transform.GlobalScale();
}

bool GameObject::IsStatic() const{
	return isStatic;
}

void GameObject::SetPosition(const Vector3& position_){
	transform.SetPosition(position_);
}

void GameObject::SetPosition(float x_, float y_, float z_){
	SetPosition(Vector3(x_, y_, z_));
}

void GameObject::SetRotation(const Euler& rotation_){
	transform.SetRotation(rotation_);
}

void GameObject::SetRotation(float x_, float y_, float z_){
	transform.SetRotation(Euler(x_, y_, z_));
}

void GameObject::SetRotation(const Matrix3& rotation_){
	transform.SetRotation(rotation_);
}

void GameObject::SetRotation(const Quaternion& rotation_){
	transform.SetRotation(rotation_);
}

void GameObject::SetScale(const Vector3& scale_){
	transform.SetScale(scale_);
}

void GameObject::SetScale(float x_, float y_, float z_){
	SetScale(Vector3(x_, y_, z_));
}

void GameObject::SetScale(float s_){
	SetScale(Vector3(s_, s_, s_));
}

void GameObject::SetGlobalPosition(const Vector3& position_){
	transform.SetGlobalPosition(position_);
}

void GameObject::SetGlobalPosition(float x_, float y_, float z_){
	SetGlobalPosition(Vector3(x_, y_, z_));
}

void GameObject::SetGlobalRotation(const Euler& rotation_){
	transform.SetGlobalRotation(rotation_);
}

void GameObject::SetGlobalRotation(float x_, float y_, float z_){
	transform.SetGlobalRotation(Euler(x_, y_, z_));
}

void GameObject::SetGlobalRotation(const Matrix3& rotation_){
	transform.SetGlobalRotation(rotation_);
}

void GameObject::SetGlobalRotation(const Quaternion& rotation_){
	transform.SetGlobalRotation(rotation_);
}

void GameObject::SetGlobalScale(const Vector3& scale_){
	transform.SetGlobalScale(scale_);
}

void GameObject::SetGlobalScale(float x_, float y_, float z_){
	SetGlobalScale(Vector3(x_, y_, z_));
}

void GameObject::SetGlobalScale(float s_){
	SetGlobalScale(Vector3(s_, s_, s_));
}

void GameObject::Translate(const Vector3& translate_){
	transform.Translate(translate_);
}

void GameObject::Translate(float x_, float y_, float z_){
	transform.Translate(x_, y_, z_);
}

void GameObject::Rotate(const Euler& rotation_){
	transform.Rotate(rotation_);
}

void GameObject::Rotate(float x_, float y_, float z_){
	transform.Rotate(x_, y_, z_);
}

void GameObject::Rotate(const Matrix3& rotation_){
	transform.Rotate(rotation_);
}

void GameObject::Rotate(const Quaternion& rotation_){
	transform.Rotate(rotation_);
}

void GameObject::SetTag(const std::string& tag_){
	tags.insert(tag_);
}

void GameObject::SetStatic(bool static_){
	isStatic = static_;
}

void GameObject::RemoveTag(const std::string& tag_){
	#ifdef _DEBUG
	if(tags.find(tag_) == tags.end()){
		Debug::LogWarning("Could not find tag: " + tag_ + "!", __FILE__, __LINE__);
		return;
	}
	#endif //_DEBUG

	tags.erase(tag_);
}

bool GameObject::HasTag(const std::string& tag_) const{
	return (tags.find(tag_) != tags.end());
}