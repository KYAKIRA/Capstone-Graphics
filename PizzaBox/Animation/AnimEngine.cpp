#include "AnimEngine.h"

#include <algorithm>

using namespace PizzaBox;

std::vector<Animator*> AnimEngine::animators;

bool AnimEngine::Initialize(){
	return true;
}

void AnimEngine::Destroy(){
	animators.clear();
	animators.shrink_to_fit();
}

void AnimEngine::Update(float deltaTime_){
	for(auto& animator : animators){
		animator->Update(deltaTime_);
	}
}

void AnimEngine::RegisterAnimator(Animator* animator_){
	_ASSERT(animator_ != nullptr);
	animators.push_back(animator_);
}

void AnimEngine::UnregisterAnimator(Animator* animator_){
	_ASSERT(animator_ != nullptr);
	animators.erase(std::remove(animators.begin(), animators.end(), animator_), animators.end());
}