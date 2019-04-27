#include "Skeleton.h"

#include <algorithm>

#include "Tools/Debug.h"

using namespace PizzaBox;

Skeleton::Skeleton() : joints(){
}

void Skeleton::AddJoint(const Joint& joint_){
	joints.push_back(joint_);
}

bool Skeleton::HasJoint(const std::string& jointName_) const{
	for(const auto& j : joints){
		if(j.name == jointName_){
			return true;
		}
	}

	return false;
}

size_t Skeleton::GetJointID(const std::string& jointName_) const{
	#ifdef _DEBUG
	if(HasJoint(jointName_) == false){
		Debug::LogError("Attempted to access invalid joint " + jointName_ + "!");
		throw std::exception("Invalid Joint Access!");
	}
	#endif //_DEBUG

	for(size_t i = 0; i < joints.size(); i++){
		if(joints[i].name == jointName_){
			return i;
		}
	}

	Debug::LogError("Attempted to access invalid joint " + jointName_ + "!");
	throw std::exception("Invalid Joint Access!");
}

const Joint& Skeleton::GetJoint(const std::string& jointName_) const{
	#ifdef _DEBUG
	if(HasJoint(jointName_) == false){
		Debug::LogError("Attempted to access invalid joint " + jointName_ + "!");
		throw std::exception("Invalid Joint Access!");
	}
	#endif //_DEBUG

	for(const auto& j : joints){
		if(j.name == jointName_){
			return j;
		}
	}

	Debug::LogError("Attempted to access invalid joint " + jointName_ + "!");
	throw std::exception("Invalid Joint Access!");
}

const Joint& Skeleton::GetJoint(unsigned int id_) const{
	_ASSERT(id_ < joints.size());
	return joints[id_];
}

std::vector<Matrix4> Skeleton::CreateInstance() const{
	std::vector<Matrix4> instance;
	instance.reserve(joints.size());

	for(unsigned int i = 0; i < joints.size(); i++){
		instance.push_back(Matrix4::Identity());
	}

	return instance;
}