#include "Vector.h"

#include <iostream>

#include <rttr/registration.h>

#include "Math.h"
#include "Plane.h"
#include "Sphere.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Vector3>("Vector3")
		.constructor()
		.constructor<float, float, float>()
		.constructor<const aiVector3D&>()
		.property("x", &Vector3::x)
		.property("y", &Vector3::y)
		.property("z", &Vector3::z)
		.method("Zero", &Vector3::Zero)
		.method("Fill", &Vector3::Fill)
		.method("__unm", static_cast<Vector3(Vector3::*)() const>(&Vector3::operator-))
		.method("__add", static_cast<Vector3(Vector3::*)(const Vector2&) const>(&Vector3::operator+))
		.method("__add", static_cast<Vector3(Vector3::*)(const Vector3&) const>(&Vector3::operator+))
		.method("__sub", static_cast<Vector3(Vector3::*)(const Vector2&) const>(&Vector3::operator-))
		.method("__sub", static_cast<Vector3(Vector3::*)(const Vector3&) const>(&Vector3::operator-))
		.method("__mul", &Vector3::operator*)
		.method("__div", &Vector3::operator/)
		.method("Magnitude", static_cast<float(Vector3::*)()const>(&Vector3::Magnitude))
		.method("Magnitude", static_cast<float(*)(const Vector3& v_)>(&Vector3::Magnitude))
		.method("Normalized", static_cast<Vector3(Vector3::*)() const>(&Vector3::Normalized))
		.method("Normalized", static_cast<Vector3(*)(const Vector3& v_)>(&Vector3::Normalized))
		.method("Normalize", &Vector3::Normalize)
		.method("Dot", &Vector3::Dot)
		.method("Cross", &Vector3::Cross)
		.method("Lerp", &Vector3::Lerp)
		.method("Rotate", &Vector3::Rotate)
		.method("Reflect", static_cast<Vector3(*)(const Vector3&, const Vector3&)>(&Vector3::Reflect))
		.method("Reflect", static_cast<Vector3(*)(const Vector3&, const Plane&)>(&Vector3::Reflect))
		.method("Distance", static_cast<float(*)(const Vector3&, const Vector3&)>(&Vector3::Distance))
		.method("Distance", static_cast<float(*)(const Vector3&, const Plane&)>(&Vector3::Distance))
		.method("Distance", static_cast<float(*)(const Sphere&, const Plane&)>(&Vector3::Distance))
		.method("ToString", &Vector3::ToString)
		.method("ToFMOD", static_cast<FMOD_VECTOR(Vector3::*)() const>(&Vector3::ToFMOD))
		.method("ToFMOD", static_cast<FMOD_VECTOR(*)(const Vector3&)>(&Vector3::ToFMOD));
}
#pragma warning( pop )

Vector3::Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_){}

Vector3::Vector3(const aiVector3D& v_) : x(v_.x), y(v_.y), z(v_.z){
}

//The Vector3 class has no pointers so this doesn't need to do anything
Vector3::~Vector3(){}

Vector3 Vector3::operator -() const{
	return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator +(const Vector2& v_) const{
	return Vector3(x + v_.x, y + v_.y, z);
}

Vector3 Vector3::operator +(const Vector3& v_) const{
	return Vector3(x + v_.x, y + v_.y, z + v_.z);
}

Vector3 Vector3::operator -(const Vector2& v_) const{
	return Vector3(x - v_.x, y - v_.y, z);
}

Vector3 Vector3::operator -(const Vector3& v_) const{
	return Vector3(x - v_.x, y - v_.y, z - v_.z);
}

Vector3 Vector3::operator *(float s_) const{
	return Vector3(x * s_, y * s_, z * s_);
}

Vector3 Vector3::operator /(float s_) const{
	if(s_ <= Math::NearZero() && s_ >= -Math::NearZero()){
		Debug::LogWarning("Attempted division by zero or near-zero! Returning a zero vector", __FILE__, __LINE__);
		return Vector3::Zero();
	}

	return Vector3(x / s_, y / s_, z / s_);
}

void Vector3::operator +=(const Vector2& v_){
	*this = *this + v_;
}

void Vector3::operator +=(const Vector3& v_){
	*this = *this + v_;
}

void Vector3::operator -=(const Vector2& v_){
	*this = *this - v_;
}

void Vector3::operator -=(const Vector3& v_){
	*this = *this - v_;
}

void Vector3::operator *=(float s_){
	*this = *this * s_;
}

void Vector3::operator /=(float s_){
	//This doesn't need to check for nearZero because the division operator already does
	//If this is changed to not use the / operator then make sure it checks for nearZero
	*this = *this / s_;
}

float Vector3::Magnitude() const{
	return sqrt((x * x) + (y * y) + (z * z));
}

float Vector3::Magnitude(const Vector3& v_){
	return v_.Magnitude();
}

Vector3 Vector3::Normalized() const{
	//This doesn't need to check for nearZero because the division operator already does
	//If this is changed to not use the / operator then make sure it checks for nearZero
	return *this / Magnitude();
}

void Vector3::Normalize(){
	*this = Normalized();
}

Vector3 Vector3::Normalized(const Vector3& v_){
	return v_.Normalized();
}

Vector3 Vector3::Rotate(const Vector3& n_, float theta_, const Vector3& v_){
	return v_ * cos(theta_) + Vector3::Dot(v_, n_) * n_ * (1.0f - cos(theta_)) + Vector3::Cross(n_, v_) * sin(theta_);
}

Vector3 Vector3::Reflect(const Vector3& v_, const Vector3& n_){
	return ((2.0f * Vector3::Dot(v_, n_)) * n_ - v_);
}

Vector3 Vector3::Reflect(const Vector3& v_, const Plane& n_){
	return ((2.0f * Vector3::Dot(v_, n_.point)) * n_.point - v_);
}

float Vector3::Distance(const Vector3& a_, const Vector3& b_){
	return Vector3::Magnitude(a_ - b_);
}

float Vector3::Distance(const Vector3& v_, const Plane& p_){
	return Vector3::Dot(v_, p_.point) - p_.d;
}

float Vector3::Distance(const Sphere& s_, const Plane& p_){
	return Distance(s_.point, p_.point) - s_.radius;
}

std::string Vector3::ToString() const{
	std::string results = std::to_string(x);
	results += ", " + std::to_string(y);
	results += ", " + std::to_string(z);
	return results;
}

float Vector3::Dot(const Vector3& a_, const Vector3& b_){
	return(a_.x * b_.x + a_.y * b_.y + a_.z * b_.z);
}

Vector3 Vector3::Cross(const Vector3& a_, const Vector3& b_){
	return Vector3(	a_.y * b_.z - a_.z * b_.y,
					a_.z * b_.x - a_.x * b_.z,
					a_.x * b_.y - a_.y * b_.x);
}

Vector3 Vector3::Lerp(const Vector3& a_, const Vector3& b_, float t_){
	return a_ + t_ * (b_ - a_);
}

Vector3::operator const float* () const{
	return static_cast<const float*>(&x);
}

Vector3::operator float* (){
	return static_cast<float*>(&x);
}

FMOD_VECTOR Vector3::ToFMOD() const{
	return FMOD_VECTOR { x, y, z };
}

FMOD_VECTOR Vector3::ToFMOD(const Vector3& v_){
	return FMOD_VECTOR { v_.x, v_.y, v_.z };
}