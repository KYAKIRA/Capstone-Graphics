#ifndef TRACKBALL_CONTROLLED_H
#define TRACKBALL_CONTROLLED_H

#include <Core/ScreenCoordinate.h>
#include <Script/Script.h>
#include <Input/Trackball.h>

namespace GamePackage{
	class TrackballControlled : public PizzaBox::Script{
	public:
		TrackballControlled();
		~TrackballControlled();

		void OnStart() override;
		void Update(const float deltaTime) override;
		void OnDestroy() override;
	private:
		bool isActive = false;
		PizzaBox::Trackball* trackball;
		PizzaBox::ScreenCoordinate screenSize;
		PizzaBox::ScreenCoordinate lastMousePos;
	};
}

#endif //!TRACKBALL_CONTROLLED_H