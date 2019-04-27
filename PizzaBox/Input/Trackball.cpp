#include "Trackball.h"

#include "../Math/Math.h"

using namespace PizzaBox;

//The math for this was taken from www.khronos.org/opengl/wiki/Object_Mouse_Trackball

Trackball::Trackball(const ScreenCoordinate& windowSize){
	SetWindowDimensions(windowSize);
}

Trackball::~Trackball(){
}

void Trackball::SetWindowDimensions(const ScreenCoordinate& windowSize){
	inverseNDC = Matrix4::ViewportNDC(windowSize.x, windowSize.y).Inverse();
}

void Trackball::OnLeftMouseDown(const ScreenCoordinate& mousePos){
	startVector = GetMouseVector(mousePos);
}

void Trackball::OnMouseMove(const ScreenCoordinate& mousePos){
	endVector = GetMouseVector(mousePos);
	const float cosAngle = Vector3::Dot(startVector, endVector);
	const float angle = Math::ConvertToDegrees(acos(cosAngle));
	const Vector3 rotAxis = Vector3::Cross(startVector, endVector);

	if((rotAxis.x > Math::NearZero() || rotAxis.x < -Math::NearZero()) && (rotAxis.y > Math::NearZero() || rotAxis.y < -Math::NearZero()) && (rotAxis.z > Math::NearZero() || rotAxis.z < -Math::NearZero())){
		mouseRotationMatrix = Matrix4::Rotate(angle, rotAxis);
	}

	startVector = endVector;
}

const Vector3 Trackball::GetMouseVector(const ScreenCoordinate& mousePos) const{
	Vector3 mousePosition(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), 0.0f);
	Vector3 v = inverseNDC * mousePosition;

	float xSquared = v.x * v.x;
	float ySquared = v.y * v.y;

	if(xSquared + ySquared <= 0.5f){
		v.z = sqrt(1.0f - (xSquared + ySquared));
	}else{
		v.z = 0.5f / sqrt(xSquared + ySquared);
		v.Normalize();
	}

	return v;
}