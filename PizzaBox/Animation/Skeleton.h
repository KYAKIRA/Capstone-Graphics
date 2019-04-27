#ifndef SKELETON_H
#define SKELETON_H

#include <vector>

#include "Joint.h"

namespace PizzaBox{
	class Skeleton{
	public:
		Skeleton();

		void AddJoint(const Joint& joint_);
		unsigned int GetJointCount() const{ return static_cast<unsigned int>(joints.size()); }
		bool HasJoint(const std::string& name_) const;
		size_t GetJointID(const std::string& name_) const;
		const Joint& GetJoint(const std::string& name_) const;
		const Joint& GetJoint(unsigned int id_) const;

		std::vector<Matrix4> CreateInstance() const;

	private:
		std::vector<Joint> joints;
	};
}

#endif //!SKELETON_H