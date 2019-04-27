#include "Animator.h"

#include <rttr/registration.h>

#include "AnimEngine.h"
#include "Resource/ResourceManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Animator>("Animator")
		.method("Initialize", &Animator::Initialize)
		.method("Destroy", &Animator::Destroy)
		.method("Update", &Animator::Update)
		.method("GetSkeleton", &Animator::GetSkeleton)
		.method("GetJointTransform", &Animator::GetJointTransform)
		.method("AddClip", &Animator::CurrentClip)
		.method("BeginTransition", &Animator::BeginTransition)
		.method("IsTransitioning", &Animator::IsTransitioning)
		.method("CurrentTime", &Animator::CurrentTime)
		.method("CurrentClip", &Animator::CurrentClip);
}
#pragma warning( pop )

Animator::Animator() : isInitialized(false), globalTime(0.0f), model(nullptr), skeleton(nullptr), clipNames(), clips(), transitionHandler(nullptr), currentClip(0){
}

Animator::~Animator(){
	#ifdef _DEBUG
	if(skeleton != nullptr || !skeletonInstance.empty() || model != nullptr || 
		!clipNames.empty() || !clips.empty() || transitionHandler != nullptr){
		Debug::LogWarning("Memory leak detected in Animator!", __FILE__, __LINE__);
		Destroy();
	}
	#endif //_DEBUG
}

bool Animator::Initialize(AnimModel* model_){
	_ASSERT(model_ != nullptr);

	model = model_;
	skeleton = model_->skeleton;
	skeletonInstance = skeleton->CreateInstance();

	for(const auto& name : clipNames){
		AnimClip* temp = ResourceManager::LoadResource<AnimClip>(name);
		if(temp == nullptr){
			return false;
		}

		clips.push_back(temp);
	}

	AnimEngine::RegisterAnimator(this);

	isInitialized = true;
	return true;
}

void Animator::Destroy(){
	isInitialized = false;
	AnimEngine::UnregisterAnimator(this);

	for(auto& name : clipNames){
		ResourceManager::UnloadResource(name);
	}

	skeletonInstance.clear();
	skeletonInstance.shrink_to_fit();

	clipNames.clear();
	clipNames.shrink_to_fit();

	clips.clear();
	clips.shrink_to_fit();

	if(skeleton != nullptr){
		//We don't own the skeleton
		skeleton = nullptr;
	}

	if(model != nullptr){
		//We don't own the model
		model = nullptr;
	}

	if(transitionHandler != nullptr){
		delete transitionHandler;
		transitionHandler = nullptr;
	}
}

void Animator::Update(float deltaTime_){
	globalTime += deltaTime_;

	if(skeleton == nullptr || model == nullptr || clips.size() <= currentClip){
		return;
	}

	if(transitionHandler == nullptr){
		while(globalTime >= clips[currentClip]->GetLength()){
			globalTime -= clips[currentClip]->GetLength();
		}

		UpdateSkeletonInstance(clips[currentClip], globalTime);
	}else{
		float blendFactor = (globalTime - transitionHandler->StartTime()) / transitionHandler->Duration();

		UpdateSkeletonInstance(clips[GetClipID(transitionHandler->StartClip())], clips[GetClipID(transitionHandler->EndClip())], globalTime, globalTime - transitionHandler->StartTime(), blendFactor);

		if(blendFactor >= 1.0f){
			globalTime = transitionHandler->Duration();
			currentClip = GetClipID(transitionHandler->EndClip());
			delete transitionHandler;
			transitionHandler = nullptr;
		}
	}
}

void Animator::UpdateSkeletonInstance(AnimClip* clip_, float time_){
	std::vector<Matrix4> globalTransforms;

	for(unsigned int i = 0; i < skeletonInstance.size(); i++){
		Matrix4 transform = clip_->GetTransformAtTime(skeleton->GetJoint(i).name, time_);

		int parentID = skeleton->GetJoint(i).parentID;
		Matrix4 parentTransform = Matrix4::Identity();
		if(parentID >= 0){
			parentTransform = globalTransforms[parentID];
		}
		
		Matrix4 globalTransform = parentTransform * transform;
		globalTransforms.push_back(globalTransform);
		skeletonInstance[i] = model->globalInverse * globalTransform * skeleton->GetJoint(i).inverseBindPose;
	}
}

void Animator::UpdateSkeletonInstance(AnimClip* clipA_, AnimClip* clipB_, float timeA_, float timeB_, float blendFactor_){
	while(timeA_ > clipA_->GetLength()){
		timeA_ -= clipA_->GetLength();
	}

	while(timeB_ > clipB_->GetLength()){
		timeB_ -= clipB_->GetLength();
	}

	std::vector<PizzaBox::Matrix4> globalTransforms;

	for(unsigned int i = 0; i < skeletonInstance.size(); i++){
		PizzaBox::Matrix4 transform = GetBlendedTransform(skeleton->GetJoint(i).name, clipA_, clipB_, timeA_, timeB_, blendFactor_);

		int parentID = skeleton->GetJoint(i).parentID;
		PizzaBox::Matrix4 parentTransform = PizzaBox::Matrix4::Identity();
		if(parentID >= 0){
			parentTransform = globalTransforms[parentID];
		}

		PizzaBox::Matrix4 globalTransform = parentTransform * transform;
		globalTransforms.push_back(globalTransform);
		skeletonInstance[i] = model->globalInverse * globalTransform * skeleton->GetJoint(i).inverseBindPose;
	}
}

Matrix4 Animator::GetBlendedTransform(const std::string& joint_, AnimClip* clip1_, AnimClip* clip2_, float clip1Time_, float clip2Time_, float blendFactor) const{
	Vector3 clip1Translate = clip1_->GetTranslateAtTime(joint_, clip1Time_);
	Quaternion clip1Rotate = clip1_->GetRotateAtTime(joint_, clip1Time_);
	Vector3 clip1Scale = clip1_->GetScaleAtTime(joint_, clip1Time_);

	Vector3 clip2Translate = clip2_->GetTranslateAtTime(joint_, clip2Time_);
	Quaternion clip2Rotate = clip2_->GetRotateAtTime(joint_, clip2Time_);
	Vector3 clip2Scale = clip2_->GetScaleAtTime(joint_, clip2Time_);

	Vector3 finalTranslate = Vector3::Lerp(clip1Translate, clip2Translate, blendFactor);
	Quaternion finalRotate = Quaternion::Lerp(clip1Rotate, clip2Rotate, blendFactor);
	Vector3 finalScale = Vector3::Lerp(clip1Scale, clip2Scale, blendFactor);

	Matrix4 finalTransform = Matrix4::Identity();
	finalTransform *= Matrix4::Translate(finalTranslate);
	finalTransform *= finalRotate.ToMatrix4();
	finalTransform *= Matrix4::Scale(finalScale);

	return finalTransform;
}

void Animator::AddClip(const std::string& clipName_){
	clipNames.push_back(clipName_);
	if(isInitialized){
		AnimClip* temp = ResourceManager::LoadResource<AnimClip>(clipName_);
		if(temp == nullptr){
			Debug::LogError("Could not load AnimClip " + clipName_ + "!");
			throw std::exception("Could not load AnimClip");
		}

		clips.push_back(temp);
	}
}

void Animator::BeginTransition(const std::string& newClip_, float duration_){
	if(transitionHandler != nullptr){
		delete transitionHandler;
		transitionHandler = nullptr;
	}

	Transition t = Transition(GetClipName(currentClip), newClip_, duration_);
	transitionHandler = new TransitionHandler(t, globalTime);
}

unsigned int Animator::GetClipID(const std::string& clipName_) const{
	for(unsigned int i = 0; i < clipNames.size(); i++){
		if(clipNames[i] == clipName_){
			return i;
		}
	}

	Debug::LogError("Invalid Clip Name!", __FILE__, __LINE__);
	throw std::exception("Invalid Animation Clip!");
}

std::string Animator::GetClipName(unsigned int id_) const{
	_ASSERT(id_ < clipNames.size());
	return clipNames[id_];
}