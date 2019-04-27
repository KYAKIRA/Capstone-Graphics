#ifndef ANIM_ENGINE_H
#define ANIM_ENGINE_H

#include "Animator.h"

namespace PizzaBox{
	class AnimEngine{
	public:
		static bool Initialize();
		static void Destroy();

		static void Update(float deltaTime_);

		static void RegisterAnimator(Animator* animator_);
		static void UnregisterAnimator(Animator* animator_);

		//Delete unwanted compiler generated constructors, destructors and assignment operators
		AnimEngine() = delete;
		AnimEngine(const AnimEngine&) = delete;
		AnimEngine(AnimEngine&&) = delete;
		AnimEngine& operator=(const AnimEngine&) = delete;
		AnimEngine& operator=(AnimEngine&&) = delete;
		~AnimEngine() = delete;

	private:
		static std::vector<Animator*> animators;
	};
}

#endif //!ANIM_ENGINE_H