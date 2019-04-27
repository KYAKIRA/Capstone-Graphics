#include "AnimClip.h"

#include <rttr/registration.h>

#include "Graphics/Models/ModelLoader.h"
#include "Math/Math.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

RTTR_REGISTRATION{
	rttr::registration::class_<PosKeyFrame>("PosKeyFrame")
		.constructor<float, Vector3>()
		.property("time", &PosKeyFrame::time)
		.property("value", &PosKeyFrame::value);

	rttr::registration::class_<RotKeyFrame>("PosKeyFrame")
		.constructor<float, Quaternion>()
		.property("time", &RotKeyFrame::time)
		.property("value", &RotKeyFrame::value);

	rttr::registration::class_<ScaleKeyFrame>("PosKeyFrame")
		.constructor<float, Vector3>()
		.property("time", &ScaleKeyFrame::time)
		.property("value", &ScaleKeyFrame::value);

	rttr::registration::class_<AnimClip>("AnimClip")
		.constructor<std::string>()
		.method("Load", &AnimClip::Load)
		.method("Unload", &AnimClip::Unload)
		.method("GetLength", &AnimClip::GetLength)
		.method("SetLength", &AnimClip::SetLength)
		.method("AddPosKey", &AnimClip::AddPosKey)
		.method("AddRotKey", &AnimClip::AddRotKey)
		.method("AddScaleKey", &AnimClip::AddScaleKey)
		.method("HasKeysForJoint", &AnimClip::HasKeysForJoint)
		.method("GetTransformAtTime", &AnimClip::GetTransformAtTime)
		.method("GetTranslateAtTime", &AnimClip::GetTranslateAtTime)
		.method("GetRotateAtTime", &AnimClip::GetRotateAtTime)
		.method("GetScaleAtTime", &AnimClip::GetScaleAtTime);
}

AnimClip::AnimClip(const std::string& filePath_) : Resource(filePath_), length(0.0f), posKeys(), rotKeys(), scaleKeys(){
}

AnimClip::~AnimClip(){
	#ifdef _DEBUG
	if(!posKeys.empty() || !rotKeys.empty() || !scaleKeys.empty()){
		Debug::LogWarning("Memory leak detected in AnimClip!", __FILE__, __LINE__);
		Unload();
	}
	#endif //_DEBUG
}

bool AnimClip::Load(){
	if(ModelLoader::LoadAnimClips(fileName, *this) == false){
		Debug::LogError("Could not load animation clip " + fileName + "!");
		return false;
	}

	return true;
}

void AnimClip::Unload(){
	posKeys.clear();
	rotKeys.clear();
	scaleKeys.clear();
}

void AnimClip::AddPosKey(const std::string& name_, const PosKeyFrame& keyFrame_){
	if(posKeys.find(name_) == posKeys.end()){
		posKeys[name_] = std::vector<PosKeyFrame>{ keyFrame_ };
	}else{
		posKeys[name_].push_back(keyFrame_);
	}
}

void AnimClip::AddRotKey(const std::string& name_, const RotKeyFrame& keyFrame_){
	if(rotKeys.find(name_) == rotKeys.end()){
		rotKeys[name_] = std::vector<RotKeyFrame>{ keyFrame_ };
	}else{
		rotKeys[name_].push_back(keyFrame_);
	}
}

void AnimClip::AddScaleKey(const std::string& name_, const ScaleKeyFrame& keyFrame_){
	if(scaleKeys.find(name_) == scaleKeys.end()){
		scaleKeys[name_] = std::vector<ScaleKeyFrame>{ keyFrame_ };
	}else{
		scaleKeys[name_].push_back(keyFrame_);
	}
}

bool AnimClip::HasKeysForJoint(const std::string& name_) const{
	return	(posKeys.find(name_) != posKeys.end()) || 
			(rotKeys.find(name_) != rotKeys.end()) || 
			(scaleKeys.find(name_) != scaleKeys.end());
}

Matrix4 AnimClip::GetTransformAtTime(const std::string& name_, float time_) const{
	Matrix4 transform = Matrix4::Identity();
	transform *= Matrix4::Translate(GetTranslateAtTime(name_, time_));
	transform *= GetRotateAtTime(name_, time_).ToMatrix4();
	transform *= Matrix4::Scale(GetScaleAtTime(name_, time_));

	return transform;
}

size_t AnimClip::FindPositionKey(const std::string& jointName_, float time_) const{
	_ASSERT(posKeys.find(jointName_) != posKeys.end());

	if(time_ > length / 2.0f){
		return BackSearch<PosKeyFrame>(posKeys.at(jointName_), time_);
	}else{
		return ForwardSearch<PosKeyFrame>(posKeys.at(jointName_), time_);
	}
}

size_t AnimClip::FindRotationKey(const std::string& jointName_, float time_) const{
	_ASSERT(rotKeys.find(jointName_) != rotKeys.end());

	if(time_ > length / 2.0f){
		return BackSearch<RotKeyFrame>(rotKeys.at(jointName_), time_);
	}else{
		return ForwardSearch<RotKeyFrame>(rotKeys.at(jointName_), time_);
	}
}

size_t AnimClip::FindScaleKey(const std::string& jointName_, float time_) const{
	_ASSERT(scaleKeys.find(jointName_) != scaleKeys.end());

	if(time_ > length / 2.0f){
		return BackSearch<ScaleKeyFrame>(scaleKeys.at(jointName_), time_);
	}else{
		return ForwardSearch<ScaleKeyFrame>(scaleKeys.at(jointName_), time_);
	}
}

Vector3 AnimClip::GetTranslateAtTime(const std::string& name_, float time_) const{
	if(posKeys.find(name_) == posKeys.end() || posKeys.at(name_).size() == 0){
		return Vector3(0.0f, 0.0f, 0.0f);
	}else if(posKeys.at(name_).size() == 1 || time_ <= 0.0f){
		return posKeys.at(name_).front().value;
	}else if(time_ >= posKeys.at(name_).back().time){
		return posKeys.at(name_).back().value;
	}

	size_t keyIndex = FindPositionKey(name_, time_);

	_ASSERT(keyIndex < posKeys.at(name_).size());

	if(keyIndex == posKeys.at(name_).size() - 1){
		return posKeys.at(name_)[keyIndex].value;
	}

	const PosKeyFrame& prevKey = posKeys.at(name_)[keyIndex];
	const PosKeyFrame& nextKey = posKeys.at(name_)[keyIndex + 1];
	return CalculateTranslation(prevKey, nextKey, time_);
}

Quaternion AnimClip::GetRotateAtTime(const std::string& name_, float time_) const{
	if(rotKeys.find(name_) == rotKeys.end() || rotKeys.at(name_).size() == 0){
		return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	}else if(rotKeys.at(name_).size() == 1 || time_ <= 0.0f){
		return rotKeys.at(name_).front().value;
	}else if(time_ >= rotKeys.at(name_).back().time){
		return rotKeys.at(name_).back().value;
	}

	size_t keyIndex = FindRotationKey(name_, time_);

	_ASSERT(keyIndex < rotKeys.at(name_).size());

	if(keyIndex == rotKeys.at(name_).size() - 1){
		return rotKeys.at(name_)[keyIndex].value;
	}

	const RotKeyFrame& prevKey = rotKeys.at(name_)[keyIndex];
	const RotKeyFrame& nextKey = rotKeys.at(name_)[keyIndex + 1];
	return CalculateRotation(prevKey, nextKey, time_);
}

Vector3 AnimClip::GetScaleAtTime(const std::string& name_, float time_) const{
	if(scaleKeys.find(name_) == scaleKeys.end() || scaleKeys.at(name_).size() == 0){
		return Vector3(1.0f, 1.0f, 1.0f);
	}else if(scaleKeys.at(name_).size() == 1 || time_ <= 0.0f){
		return scaleKeys.at(name_).front().value;
	}else if(time_ >= scaleKeys.at(name_).back().time){
		return scaleKeys.at(name_).back().value;
	}

	size_t keyIndex = FindScaleKey(name_, time_);

	_ASSERT(keyIndex < scaleKeys.at(name_).size());

	if(keyIndex == scaleKeys.at(name_).size() - 1){
		return scaleKeys.at(name_)[keyIndex].value;
	}

	const ScaleKeyFrame& prevKey = scaleKeys.at(name_)[keyIndex];
	const ScaleKeyFrame& nextKey = scaleKeys.at(name_)[keyIndex + 1];
	return CalculateScale(prevKey, nextKey, time_);
}

Vector3 AnimClip::CalculateTranslation(const PosKeyFrame& prevKey_, const PosKeyFrame& nextKey_, float time_) const{
	float deltaTime = nextKey_.time - prevKey_.time;
	float factor = (time_ - prevKey_.time) / deltaTime;
	factor = Math::Clamp(0.0f, 1.0f, factor);

	return Vector3::Lerp(prevKey_.value, nextKey_.value, factor);
}

Quaternion AnimClip::CalculateRotation(const RotKeyFrame& prevKey_, const RotKeyFrame& nextKey_, float time_) const{
	float deltaTime = nextKey_.time - prevKey_.time;
	float factor = (time_ - prevKey_.time) / deltaTime;
	factor = Math::Clamp(0.0f, 1.0f, factor);

	return Quaternion::Lerp(prevKey_.value, nextKey_.value, factor);
}

Vector3 AnimClip::CalculateScale(const ScaleKeyFrame& prevKey_, const ScaleKeyFrame& nextKey_, float time_) const{
	float deltaTime = nextKey_.time - prevKey_.time;
	float factor = (time_ - prevKey_.time) / deltaTime;
	factor = Math::Clamp(0.0f, 1.0f, factor);

	return Vector3::Lerp(prevKey_.value, nextKey_.value, factor);
}