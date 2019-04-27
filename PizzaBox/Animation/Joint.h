#ifndef JOINT_H
#define JOINT_H

#include <string>

#include "Math/Matrix.h"

namespace PizzaBox{
	struct Joint{
		Joint() : name(""), parentID(-1), inverseBindPose(){
		}

		std::string name;
		int parentID;
		Matrix4 inverseBindPose;
	};
}

#endif //!JOINT_H