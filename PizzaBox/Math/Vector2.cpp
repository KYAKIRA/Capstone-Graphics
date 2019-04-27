#include "Vector.h"

#include <rttr/registration.h>

#include "Math.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Vector2>("Vector2")
		.constructor()
		.constructor<float, float>()
		.property("x", &Vector2::x)
		.property("y", &Vector2::y)
		.method("Zero", &Vector2::Zero)
		.method("Fill", &Vector2::Fill)
		.method("__unm", static_cast<Vector2(Vector2::*)() const>(&Vector2::operator-))
		.method("__add", &Vector2::operator+)
		.method("__sub", static_cast<Vector2(Vector2::*)(const Vector2&) const>(&Vector2::operator-))
		.method("__mul", &Vector2::operator*)
		.method("__div", &Vector2::operator/)
		.method("Magnitude", static_cast<float(Vector2::*)()const>(&Vector2::Magnitude))
		.method("Magnitude", static_cast<float(*)(const Vector2& v_)>(&Vector2::Magnitude))
		.method("Normalized", static_cast<Vector2(Vector2::*)() const>(&Vector2::Normalized))
		.method("Normalized", static_cast<Vector2(*)(const Vector2& v_)>(&Vector2::Normalized))
		.method("Normalize", &Vector2::Normalize)
		.method("Dot", &Vector2::Dot)
		.method("ToString", &Vector2::ToString);
}
#pragma warning( pop )

Vector2::Vector2(float x_, float y_) : x(x_), y(y_){}

//This class has no pointers so this doesn't need to do anything
Vector2::~Vector2(){}

Vector2 Vector2::operator -() const{
	return Vector2(-x, -y);
}

Vector2 Vector2::operator +(const Vector2& v_) const{
	return Vector2(x + v_.x, y + v_.y);
}

Vector2 Vector2::operator -(const Vector2& v_) const{
	return Vector2(x - v_.x, y - v_.y);
}

Vector2 Vector2::operator *(float s_) const{
	return Vector2(x * s_, y * s_);
}

Vector2 Vector2::operator /(float s_) const{
	if(s_ <= Math::NearZero() && s_ >= -Math::NearZero()){
		Debug::LogWarning("Attempted division by zero or near-zero! Returning a zero vector", __FILE__, __LINE__);
		return Vector2::Zero();
	}

	return Vector2(x / s_, y / s_);
}

void Vector2::operator +=(const Vector2& v_){
	*this = *this + v_;
}

void Vector2::operator -=(const Vector2& v_){
	*this = *this - v_;
}

void Vector2::operator *=(float s_){
	*this = *this * s_;
}

void Vector2::operator /=(float s_){
	//This doesn't need to check for nearZero because the division operator already does
	//If this is changed to not use the / operator then make sure it checks for nearZero
	*this = *this / s_;
}

float Vector2::Magnitude() const{
	return sqrt((x * x) + (y * y));
}

float Vector2::Magnitude(const Vector2& v_){
	return v_.Magnitude();
}

Vector2 Vector2::Normalized() const{
	//This doesn't need to check for nearZero because the division operator already does
	//If this is changed to not use the / operator then make sure it checks for nearZero
	return *this / Magnitude();
}

void Vector2::Normalize(){
	*this = Normalized();
}

Vector2 Vector2::Normalized(const Vector2& v_){
	return v_.Normalized();
}

float Vector2::Dot(const Vector2& a_, const Vector2& b_){
	return (a_.x * b_.x) + (a_.y * b_.y);
}

std::string Vector2::ToString() const{
	std::string result = std::to_string(x);
	result += ", " + std::to_string(y);
	return result;
}

Vector2::operator const float* () const{
	return static_cast<const float*>(&x);
}

Vector2::operator float* (){
	return static_cast<float*>(&x);
}