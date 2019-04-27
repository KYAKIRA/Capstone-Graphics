#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Math/Euler.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/Vector.h"

namespace PizzaBox{
	class Transform{
	public:
		Transform(Vector3 pos_ = Vector3(0.0f, 0.0f, 0.0f), Euler rot_ = Euler(0.0f, 0.0f, 0.0f), Vector3 scale_ = Vector3(1.0f, 1.0f, 1.0f));
		Transform(Transform* parent_, Vector3 pos_ = Vector3(0.0f, 0.0f, 0.0f), Euler rot_ = Euler(0.0f, 0.0f, 0.0f), Vector3 scale_ = Vector3(1.0f, 1.0f, 1.0f));
		~Transform();

		static const Vector3 worldForward;
		static const Vector3 worldUp;
		static const Vector3 worldRight;

		Matrix4 GetTransformation() const;

		void Translate(const Vector3& translation_);
		void Translate(float x_, float y_, float z_);
		void Rotate(const Euler& rotation_);
		void Rotate(float x_, float y_, float z_);
		void Rotate(const Matrix3& rotation_);
		void Rotate(const Quaternion& rotation_);

		Vector3 GlobalPosition() const;
		Euler GlobalRotation() const;
		Quaternion GlobalRotationQuat() const;
		Vector3 GlobalScale() const;

		Transform* GetParent() const;
		Vector3 GetPosition() const;
		Euler GetRotation() const;
		Quaternion GetRotationQuat() const;
		Vector3 GetScale() const;

		Vector3 GetForward() const;
		Vector3 GetUp() const;
		Vector3 GetRight() const;
		
		void SetInitialParent(Transform* parent_);
		void SetParent(Transform* parent_);
		void SetPosition(const Vector3& position_);
		void SetPosition(const float x, const float y, const float z);
		void SetRotation(const Euler& rotation_);
		void SetRotation(const float x, const float y, const float z);
		void SetRotation(const Matrix3& rotation_);
		void SetRotation(const Quaternion& rotation_);
		void SetScale(const Vector3& scale_);
		void SetScale(const float x, const float y, const float z);
		void SetScale(const float s);

		void SetGlobalPosition(const Vector3& position_);
		void SetGlobalPosition(const float x, const float y, const float z);
		void SetGlobalRotation(const Euler& rotation_);
		void SetGlobalRotation(const float x, const float y, const float z);
		void SetGlobalRotation(const Matrix3& rotation_);
		void SetGlobalRotation(const Quaternion& rotation_);
		void SetGlobalScale(const Vector3& scale_);
		void SetGlobalScale(const float x, const float y, const float z);
		void SetGlobalScale(const float s);

	private:
		Transform* parent;

		Vector3 localPosition;
		Quaternion localRotation;
		Vector3 localScale;

		Vector3 forward;
		Vector3 up;
		Vector3 right;

		void CalculateDirectionVectors();
	};
}

#endif //!TRANSFORM_H