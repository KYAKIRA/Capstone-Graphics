#ifndef TRACKBALL_H
#define	TRACKBALL_H

#include "../Core/Window.h"
#include "../Math/Matrix.h"

namespace PizzaBox{
	class Trackball{
	public:
		Trackball(const ScreenCoordinate& windowSize);
		~Trackball();

		//Any time the window dimensions change we'll need to rebuild the inverseNDC matrix
		void SetWindowDimensions(const ScreenCoordinate& windowSize);

		void OnLeftMouseDown(const ScreenCoordinate& mousePos);
		void OnMouseMove(const ScreenCoordinate& mousePos);

		//Return the 4x4 rotation matrix
		const Matrix4 GetMatrix4() const{
			return mouseRotationMatrix;
		}

		//Return the 3x3 rotation matrix
		const Matrix3 GetMatrix3() const{
			return Matrix3(mouseRotationMatrix); 
		}

	private:
		Matrix4 mouseRotationMatrix;	//This the final rotation matrix
		Matrix4 inverseNDC;				//Inverse of the viewportNDC matrix
		Vector3 startVector, endVector; //Begin and end points after being transformed by inverseNDC

		const Vector3 GetMouseVector(const ScreenCoordinate& mousePos) const;
	};

}

#endif //!TRACKBALL_H