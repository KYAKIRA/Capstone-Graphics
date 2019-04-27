#ifndef RANDOM_H
#define RANDOM_H

#include "../Graphics/Color.h"

namespace PizzaBox {
	class Random {
	public:
		static void SetSeed();
		static int Range(const int min, const int max);
		static float Range(const float min, const float max);
		static Color RandomColor();
	};
}
#endif //!RANDOM_H