#ifndef SHADOW_BOX_H
#define SHADOW_BOX_H

#include <vector>

#include "Math/Matrix.h"
#include "Math/Vector.h"

namespace PizzaBox{
	//Forward declarations
	class Camera;

	class ShadowBox{
	public:
		ShadowBox(float shadowDistance_ = 100.0f);
		~ShadowBox();
		
		void UpdateBox(Camera* camera_);
		Vector3 GetCenter();
		 
		inline float GetWidth(){ return max.x - min.x; }
		inline float GetHeight(){ return max.y - min.y; }
		inline float GetLength(){ return max.z - min.z; }

		inline void SetLightViewMatrix(const Matrix4& lvm_) { lightViewMatrix = lvm_; }

	private:
		const float offset;
		const float shadowDistance;
		const Vector3 up;
		const Vector3 forward;
		float farHeight, farWidth, nearHeight, nearWidth;
		Vector3 min;
		Vector3 max;
		Matrix4 lightViewMatrix;

		std::vector<Vector4> CalculateFrustumVertices(const Matrix4& rot_, const Vector3& forward_, const Vector3& near_, const Vector3& far_);
		Vector4 CalculateLightSpaceFrustumCorner(const Vector3& startPoint_, const Vector3& direction_, float width_);
	}; 
}

#endif //!SHADOW_BOX_H