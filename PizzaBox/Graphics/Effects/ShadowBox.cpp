#include "ShadowBox.h"

#include "Graphics/Camera.h"
#include "Graphics/RenderEngine.h"
#include "Math/Math.h"

using namespace PizzaBox;

ShadowBox::ShadowBox(float shadowDistance_) : offset(10.0f), shadowDistance(shadowDistance_), up(0.0f, 1.0f, 0.0f), forward(0.0f, 0.0f, -1.0f), lightViewMatrix(Matrix4::Identity()){
	float fov = 45.0f;
	float nearPlane = 0.1f;
	float aspect = static_cast<float>(RenderEngine::ScreenSize().x) / static_cast<float>(RenderEngine::ScreenSize().y);
	farWidth = shadowDistance * Math::Tan(fov);
	nearWidth = nearPlane * Math::Tan(fov);
	farHeight = farWidth / aspect;
	nearHeight = nearWidth / aspect;
}

ShadowBox::~ShadowBox(){
}

void ShadowBox::UpdateBox(Camera* camera_){
	Matrix4 rotation = camera_->GetGameObject()->GlobalRotationQuat().ToMatrix4();
	Vector3 forwardVector = camera_->GetGameObject()->GlobalRotationQuat().ToMatrix4() * forward;

	Vector3 toFar = forwardVector;
	toFar = toFar * shadowDistance;
	Vector3 toNear = forwardVector;
	toNear *= camera_->GetNearPlane();
	Vector3 centerNear = toNear + camera_->GetGameObject()->GetPosition();
	Vector3 centerFar = toFar + camera_->GetGameObject()->GetPosition();

	std::vector<Vector4> points = CalculateFrustumVertices(rotation, forwardVector, centerNear, centerFar);

	bool first = true;
	for(Vector4 point : points){
		if(first){
			min.x = point.x;
			max.x = point.x;
			min.y = point.y;
			max.y = point.y;
			min.z = point.z;
			max.z = point.z;
			first = false;
			continue;
		}

		if(point.x > max.x){
			max.x = point.x;
		}else if (point.x < min.x){
			min.x = point.x;
		}

		if(point.y > max.y){
			max.y = point.y;
		}else if (point.y < min.y){
			min.y = point.y;
		}

		if(point.z > max.z){
			max.z = point.z;
		}else if (point.z < min.z){
			min.z = point.z;
		}
	}
	
	max.z += offset;
}
 
Vector3 ShadowBox::GetCenter(){
	/*
		Calculates the center of the "view cuboid" in light space first, and then
		converts this to world space using the inverse light's view matrix.

		return The center of the "view cuboid" in world space.
	*/
	Vector3 center = (min + max) / 2.0f;
	return lightViewMatrix.Inverse() * center;
}

std::vector<Vector4> ShadowBox::CalculateFrustumVertices(const Matrix4& rot_, const Vector3& forward_, const Vector3& near_, const Vector3& far_){
	Vector3 upVector = rot_ * up;
	Vector3 rightVector = Vector3::Cross(forward_, upVector);
	Vector3 downVector = -upVector;
	Vector3 leftVector = -rightVector;
	Vector3 farTop = far_ + (upVector * farHeight);
	Vector3 farBottom = far_ + (downVector * farHeight);
	Vector3 nearTop = near_ + (upVector * nearHeight);
	Vector3 nearBottom = near_ + (downVector * nearHeight);

	std::vector<Vector4> points;
	points.push_back(CalculateLightSpaceFrustumCorner(farTop, rightVector, farWidth));
	points.push_back(CalculateLightSpaceFrustumCorner(farTop, leftVector, farWidth));
	points.push_back(CalculateLightSpaceFrustumCorner(farBottom, rightVector, farWidth));
	points.push_back(CalculateLightSpaceFrustumCorner(farBottom, leftVector, farWidth));
	points.push_back(CalculateLightSpaceFrustumCorner(nearTop, rightVector, nearWidth));
	points.push_back(CalculateLightSpaceFrustumCorner(nearTop, leftVector, nearWidth));
	points.push_back(CalculateLightSpaceFrustumCorner(nearBottom, rightVector, nearWidth));
	points.push_back(CalculateLightSpaceFrustumCorner(nearBottom, leftVector, nearWidth));
	return points;
}

/*
	 Calculates one of the corner vertices of the view frustum in world space
	 and converts it to light space.
	
	 param startPoint - the starting center point on the view frustum.
	 param direction - the direction of the corner from the start point.
	 param width - the distance of the corner from the start point.

	 return - The relevant corner vertex of the view frustum in light space.
*/
Vector4 ShadowBox::CalculateLightSpaceFrustumCorner(const Vector3& startPoint_, const Vector3& direction_, float width_){
	Vector3 point = startPoint_ + (direction_ * width_);
	return lightViewMatrix * Vector4(point.x, point.y, point.z, 1.0f);
}