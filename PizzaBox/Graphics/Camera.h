#ifndef CAMERA_H
#define CAMERA_H

#include <glew.h>

#include "ViewportRect.h"
#include "Math/Matrix.h"
#include "Object/GameObject.h"

namespace PizzaBox{
	class Camera : public Component{
	public:
		enum class RenderMode{
			Orthographic,
			Perspective
		};

		Camera(const ViewportRect& vr_ = ViewportRect::fullScreen, RenderMode mode_ = RenderMode::Perspective);
		~Camera();

		bool Initialize(GameObject* go_) override;
		void Destroy() override;

		void Reset();

		void CalculateViewMatrix();

		//Getters
		Matrix4& GetProjectionMatrix() const;
		Matrix4 GetViewMatrix() const;
		Matrix4 GetPerspective() const;
		Matrix4 GetOrthographic() const;
		ViewportRect GetViewportRect() const;
		RenderMode GetRenderMode() const;
		inline float GetNearPlane() const{ return nearPlane; }
		inline float GetFarPlane() const{ return farPlane; }

		//Setters
		void SetRenderMode(const RenderMode switchMode_);
		void SetViewportRect(const ViewportRect& vr_);
		void SetFOV(float fov_);
		void SetNearPlane(float near_);
		void SetFarPlane(float far_);

	private:
		Matrix4 perspective, orthographic;
		Matrix4 viewMatrix;
		Matrix4& projectionMatrix;

		GLfloat fieldOfView, nearPlane, farPlane;

		RenderMode renderMode;
		ViewportRect viewportRect;
	};
}

#endif //!CAMERA_H