#include "Camera.h"

#include <rttr/registration.h>

#include "RenderEngine.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Camera>("Camera")
		.enumeration<Camera::RenderMode>("RenderMode")(
			rttr::value("Orthographic", Camera::RenderMode::Orthographic),
			rttr::value("Perspective", Camera::RenderMode::Perspective)
		)
		.constructor<const ViewportRect&, Camera::RenderMode>()
		.method("Initialize", &Camera::Initialize)
		.method("Destroy", &Camera::Destroy)
		.method("Reset", &Camera::Reset)
		.method("GetProjectionMatrix", &Camera::GetProjectionMatrix)
		.method("GetViewMatrix", &Camera::GetViewMatrix)
		.method("GetPerspective", &Camera::GetPerspective)
		.method("GetOrthographic", &Camera::GetOrthographic)
		.method("GetViewportRect", &Camera::GetViewportRect)
		.method("GetRenderMode", &Camera::GetRenderMode)
		.method("SetRenderMode", &Camera::SetRenderMode)
		.method("SetViewportRect", &Camera::SetViewportRect)
		.method("SetFOV", &Camera::SetFOV);
}
#pragma warning( pop )

Camera::Camera(const ViewportRect& vr_, RenderMode mode_) : Component(), perspective(Matrix4()), orthographic(Matrix4()), viewMatrix(Matrix4()),
	fieldOfView(45.0f), nearPlane(0.1f), farPlane(1000.0f), projectionMatrix(perspective), renderMode(mode_), viewportRect(vr_){
}

Camera::~Camera(){}

bool Camera::Initialize(GameObject* go_){
	//Make sure the GameObject is a valid pointer
	_ASSERT(go_ != nullptr);

	gameObject = go_;
	Reset();
	RenderEngine::RegisterCamera(this);
	return true;
}

void Camera::Destroy(){
	RenderEngine::UnregisterCamera(this);
	gameObject = nullptr;
}

void Camera::Reset(){
	float aspect = static_cast<float>(RenderEngine::ScreenSize().x * viewportRect.width) / static_cast<float>(RenderEngine::ScreenSize().y * viewportRect.height);
	perspective = Matrix4::Perspective(fieldOfView, aspect, nearPlane, farPlane);
	orthographic = Matrix4::Orthographic(-10.0f, 10.0f * aspect, -10.0f, 10.0f / aspect, -100.0f, 100.0f);
	SetRenderMode(renderMode);
}

void Camera::CalculateViewMatrix(){
	viewMatrix = Matrix4::Identity();
	viewMatrix *= gameObject->GlobalRotationQuat().ToMatrix4().Inverse();
	viewMatrix *= Matrix4::Translate(gameObject->GetTransform()->GlobalPosition()).Inverse();
}

Matrix4& Camera::GetProjectionMatrix() const{
	return projectionMatrix;
}

Matrix4 Camera::GetViewMatrix() const{
	return viewMatrix;
}

Matrix4 Camera::GetPerspective() const{
	return perspective;
}

Matrix4 Camera::GetOrthographic() const{
	return orthographic;
}

ViewportRect Camera::GetViewportRect() const{
	return viewportRect;
}

Camera::RenderMode Camera::GetRenderMode() const{
	return renderMode;
}

void Camera::SetRenderMode(const RenderMode switchMode){
	renderMode = switchMode;
	if(renderMode == RenderMode::Orthographic){
		projectionMatrix = orthographic;
	}else if(renderMode == RenderMode::Perspective){
		projectionMatrix = perspective;
	}
}

void Camera::SetViewportRect(const ViewportRect& vr_){
	viewportRect = vr_;
	Reset();
}

void Camera::SetFOV(float fov_){
	fieldOfView = fov_;
	Reset();
}

void Camera::SetNearPlane(float near_){
	nearPlane = near_;
	Reset();
}

void Camera::SetFarPlane(float far_){
	farPlane = far_;
	Reset();
}