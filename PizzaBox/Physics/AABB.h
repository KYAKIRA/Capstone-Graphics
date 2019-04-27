#ifndef AABB_H
#define AABB_H

#include "../Math/Vector.h"

namespace PizzaBox{
	struct AABB{
		AABB(const Vector3& pos_, const Vector3& scale_) : lower(pos_ - scale_), upper(pos_ + scale_){
		}

		Vector3 lower;
		Vector3 upper;
	};
}

#endif //!AABB_H