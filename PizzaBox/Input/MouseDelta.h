#ifndef MOUSE_DELTA_H
#define MOUSE_DELTA_H

#include "../Core/ScreenCoordinate.h"

namespace PizzaBox{
	struct MouseDelta{
		MouseDelta(int x_ = 0, int y_ = 0) : x(x_), y(y_){
		}

		int x;
		int y;

		MouseDelta CalculateDifference(const ScreenCoordinate& old_, const ScreenCoordinate& new_){
			return MouseDelta(new_.x - old_.x, new_.y - old_.y);
		}
	};
}

#endif //!MOUSE_DELTA_H