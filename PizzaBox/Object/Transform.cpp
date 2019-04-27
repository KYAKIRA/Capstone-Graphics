#include "Transform.h"

#include <rttr/registration.h>

#include "Math/Matrix.h"

using namespace PizzaBox;

const Vector3 Transform::worldForward = Vector3(0.0f, 0.0f, -1.0f);
const Vector3 Transform::worldUp =		Vector3(0.0f, 1.0f, 0.0f);
const Vector3 Transform::worldRight =	Vector3(1.0f, 0.0f, 0.0f);

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Transform>("Transform")
		.constructor()
		.property_readonly("worldForward", &Transform::worldForward)
		.property_readonly("worldUp", &Transform::worldUp)
		.property_readonly("worldRight", &Transform::worldRight)
		.method("GetTransformation", &Transform::GetTransformation)
		.method("Translate", static_cast<void(Transform::*)(const Vector3&)>(&Transform::Translate))
		.method("Translate", static_cast<void(Transform::*)(float, float, float)>(&Transform::Translate))
		.method("Rotate", static_cast<void(Transform::*)(const Euler&)>(&Transform::Rotate))
		.method("Rotate", static_cast<void(Transform::*)(float, float, float)>(&Transform::Rotate))
		.method("Rotate", static_cast<void(Transform::*)(const Matrix3&)>(&Transform::Rotate))
		.method("Rotate", static_cast<void(Transform::*)(const Quaternion&)>(&Transform::Rotate))
		.method("GlobalPosition", &Transform::GlobalPosition)
		.method("GlobalRotation", &Transform::GlobalRotation)
		.method("GlobalRotationQuat", &Transform::GlobalRotationQuat)
		.method("GlobalScale", &Transform::GlobalScale)
		.method("GetParent", &Transform::GetParent)
		.method("GetPosition", &Transform::GetPosition)
		.method("GetRotation", &Transform::GetRotation)
		.method("GetRotationQuat", &Transform::GetRotationQuat)
		.method("GetScale", &Transform::GetScale)
		.method("GetForward", &Transform::GetForward)
		.method("GetUp", &Transform::GetUp)
		.method("GetRight", &Transform::GetRight)
		.method("SetInitialParent", &Transform::SetInitialParent)
		.method("SetParent", &Transform::SetParent)
		.method("SetPosition", static_cast<void(Transform::*)(const Vector3&)>(&Transform::SetPosition))
		.method("SetPosition", static_cast<void(Transform::*)(float, float, float)>(&Transform::SetPosition))
		.method("SetRotation", static_cast<void(Transform::*)(const Euler&)>(&Transform::SetRotation))
		.method("SetRotation", static_cast<void(Transform::*)(float, float, float)>(&Transform::SetRotation))
		.method("SetRotation", static_cast<void(Transform::*)(const Matrix3&)>(&Transform::SetRotation))
		.method("SetRotation", static_cast<void(Transform::*)(const Quaternion&)>(&Transform::SetRotation))
		.method("SetScale", static_cast<void(Transform::*)(const Vector3&)>(&Transform::SetScale))
		.method("SetScale", static_cast<void(Transform::*)(float, float, float)>(&Transform::SetScale))
		.method("SetScale", static_cast<void(Transform::*)(float)>(&Transform::SetScale))
		.method("SetGlobalPosition", static_cast<void(Transform::*)(const Vector3&)>(&Transform::SetGlobalPosition))
		.method("SetGlobalPosition", static_cast<void(Transform::*)(float, float, float)>(&Transform::SetGlobalPosition))
		.method("SetGlobalRotation", static_cast<void(Transform::*)(const Euler&)>(&Transform::SetGlobalRotation))
		.method("SetGlobalRotation", static_cast<void(Transform::*)(float, float, float)>(&Transform::SetGlobalRotation))
		.method("SetGlobalRotation", static_cast<void(Transform::*)(const Matrix3&)>(&Transform::SetGlobalRotation))
		.method("SetGlobalRotation", static_cast<void(Transform::*)(const Quaternion&)>(&Transform::SetGlobalRotation))
		.method("SetGlobalScale", static_cast<void(Transform::*)(const Vector3&)>(&Transform::SetGlobalScale))
		.method("SetGlobalScale", static_cast<void(Transform::*)(float, float, float)>(&Transform::SetGlobalScale))
		.method("SetGlobalScale", static_cast<void(Transform::*)(float)>(&Transform::SetGlobalScale));
}
#pragma warning( pop )

Transform::Transform(Vector3 pos, Euler rot, Vector3 scale) : parent(nullptr), localPosition(pos), localRotation(rot.ToQuaternion()), localScale(scale), forward(worldForward), up(worldUp), right(worldRight){
	CalculateDirectionVectors();
}

Transform::Transform(Transform* parent_, Vector3 pos, Euler rot, Vector3 scale) : parent(parent_), localPosition(pos), localRotation(rot.ToQuaternion()), localScale(scale), forward(worldForward), up(worldUp), right(worldRight){
	CalculateDirectionVectors();
}

Transform::~Transform(){
}

Matrix4 Transform::GetTransformation() const{
	//Translation Matrix
	Matrix4 positionMatrix = Matrix4::Translate(localPosition);

	//Rotation Matrix
	Matrix4 rotationMatrix = localRotation.ToMatrix4();

	//Scale Matrix
	Matrix4 scaleMatrix = Matrix4::Scale(localScale);
	
	if(parent == nullptr){
		return (positionMatrix * (rotationMatrix * scaleMatrix));
	}else{
		return parent->GetTransformation() * (positionMatrix * (rotationMatrix * scaleMatrix));
	}
}

void Transform::Translate(const Vector3& translation_){
	localPosition += translation_;
}

void Transform::Translate(float x_, float y_, float z_){
	Translate(Vector3(x_, y_, z_));
}

void Transform::Rotate(const Euler& rotation_){
	Rotate(rotation_.ToQuaternion());
}

void Transform::Rotate(float x_, float y_, float z_){
	Rotate(Euler(x_, y_, z_).ToQuaternion());
}

void Transform::Rotate(const Matrix3& rotation_){
	Rotate(rotation_.ToQuaternion());
}

//All other Rotate overloads should use this
void Transform::Rotate(const Quaternion& rotation_){
	localRotation *= rotation_;
	CalculateDirectionVectors();
}

Vector3 Transform::GlobalPosition() const{
	if(parent == nullptr){
		return localPosition;
	}else{
		return localPosition + parent->GlobalPosition();
	}
}

Euler Transform::GlobalRotation() const{
	return GlobalRotationQuat().ToEuler();
}

Quaternion Transform::GlobalRotationQuat() const{
	if(parent == nullptr){
		return localRotation;
	}else{
		return localRotation * parent->GlobalRotationQuat();
	}
}

Vector3 Transform::GlobalScale() const{
	if(parent == nullptr){
		return localScale;
	}else{
		Vector3 pG = parent->GlobalScale();
		return Vector3(localScale.x * pG.x, localScale.y * pG.y, localScale.z * pG.z);
	}
}

Transform* Transform::GetParent() const{
	return parent;
}

Vector3 Transform::GetPosition() const{
	return localPosition;
}

Euler Transform::GetRotation() const{
	return localRotation.ToEuler();
}

Quaternion Transform::GetRotationQuat() const{
	return localRotation;
}

Vector3 Transform::GetScale() const{
	return localScale;
}

Vector3 Transform::GetForward() const{
	return forward;
}

Vector3 Transform::GetUp() const{ 
	return up; 
}

Vector3 Transform::GetRight() const{ 
	return right;
}

void Transform::SetInitialParent(Transform* parent_){
	parent = parent_;
}

void Transform::SetParent(Transform* parent_){
	if(parent == parent_){
		return;
	}

	auto oldGlobalPos = GlobalPosition();
	auto oldGlobalRot = GlobalRotation();
	auto oldGlobalScale = GlobalScale();

	parent = parent_;

	SetGlobalPosition(oldGlobalPos);
	SetGlobalRotation(oldGlobalRot);
	SetGlobalScale(oldGlobalScale);
}

void Transform::SetPosition(const Vector3& position_){
	localPosition = position_;
}

void Transform::SetPosition(float x_, float y_, float z_){
	localPosition = Vector3(x_, y_, z_);
}

void Transform::SetRotation(const Euler& rotation_){
	SetRotation(rotation_.ToQuaternion());
}

void Transform::SetRotation(float x_, float y_, float z_){
	SetRotation(Euler(x_, y_, z_).ToQuaternion());
}

void Transform::SetRotation(const Matrix3& rotation_){
	SetRotation(rotation_.ToQuaternion());
}

//All other SetRotation overloads should use this
void Transform::SetRotation(const Quaternion& rotation_){
	localRotation = rotation_;
	CalculateDirectionVectors();
}

void Transform::SetScale(const Vector3& scale_){
	localScale = scale_;
}

void Transform::SetScale(float x_, float y_, float z_){
	SetScale(Vector3(x_, y_, z_));
}

void Transform::SetScale(float s_){
	SetScale(Vector3(s_, s_, s_));
}

void Transform::SetGlobalPosition(const Vector3& position_){
	if(parent == nullptr){
		SetPosition(position_);
	}else{
		localPosition = position_ - parent->GlobalPosition();
	}
}

void Transform::SetGlobalPosition(float x_, float y_, float z_){
	SetGlobalPosition(Vector3(x_, y_, z_));
}

void Transform::SetGlobalRotation(const Euler& rotation_){
	SetGlobalRotation(rotation_.ToQuaternion());
}

void Transform::SetGlobalRotation(float x_, float y_, float z_){
	SetGlobalRotation(Euler(x_, y_, z_).ToQuaternion());
}

void Transform::SetGlobalRotation(const Matrix3& rotation_){
	SetGlobalRotation(rotation_.ToQuaternion());
}

//All other SetGlobalRotation overloads should use this
void Transform::SetGlobalRotation(const Quaternion& rotation_){
	if(parent == nullptr){
		localRotation = rotation_;
	}else{
		localRotation = rotation_ * parent->GlobalRotationQuat().Inverse();
	}

	CalculateDirectionVectors();
}

void Transform::SetGlobalScale(const Vector3& scale_){
	if(parent == nullptr){
		localScale = scale_;
	}else{
		localScale.x = scale_.x / parent->GlobalScale().x;
		localScale.y = scale_.y / parent->GlobalScale().y;
		localScale.z = scale_.z / parent->GlobalScale().z;
	}
}

void Transform::SetGlobalScale(float x_, const float y_, const float z_){
	SetGlobalScale(Vector3(x_, y_, z_));
}

void Transform::SetGlobalScale(float s_){
	SetGlobalScale(Vector3(s_, s_, s_));
}

void Transform::CalculateDirectionVectors(){
	Matrix4 rotationMatrix = GlobalRotation().ToMatrix4();

	forward = rotationMatrix * worldForward;
	up = rotationMatrix * worldUp;
	right = rotationMatrix * worldRight;
}