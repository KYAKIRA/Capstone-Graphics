#ifndef SCREEN_COORDINATE_H
#define SCREEN_COORDINATE_H

namespace PizzaBox{
	struct ScreenCoordinate{
		unsigned int x;
		unsigned int y;

		explicit ScreenCoordinate(const unsigned int x_ = 0, const unsigned int y_ = 0);
		~ScreenCoordinate();

		bool operator ==(const ScreenCoordinate& sc) const;
		bool operator !=(const ScreenCoordinate& sc) const;

		ScreenCoordinate operator +(const ScreenCoordinate& sc) const;
		ScreenCoordinate operator -(const ScreenCoordinate& sc) const;

		void operator +=(const ScreenCoordinate& sc);
		void operator -=(const ScreenCoordinate& sc);
	};
}

#endif //!SCREEN_COORDINATE_H