#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "AnimClip.h"
#include "AnimModel.h"
#include "Skeleton.h"
#include "TransitionHandler.h"

namespace PizzaBox{
	class Animator{
	public:
		Animator();
		virtual ~Animator();

		virtual bool Initialize(AnimModel* model_);
		virtual void Destroy();

		virtual void Update(float deltaTime_);

		inline Skeleton* GetSkeleton() const{ return skeleton; }
		
		inline Matrix4 GetJointTransform(size_t jointID_) const{
			_ASSERT(jointID_ < skeletonInstance.size());
			return skeletonInstance[jointID_];
		}
		
		void AddClip(const std::string& clipName_);

		void BeginTransition(const std::string& newClip_, float duration_);

		inline bool IsTransitioning() const{ return (transitionHandler != nullptr); }

		inline float CurrentTime(){ return globalTime; }

		inline AnimClip* CurrentClip(){
			_ASSERT(currentClip < clips.size());
			return clips[currentClip];
		}

	protected:
		bool isInitialized;
		float globalTime;
		Skeleton* skeleton;
		std::vector<Matrix4> skeletonInstance;
		AnimModel* model;
		std::vector<std::string> clipNames;
		std::vector<AnimClip*> clips;
		TransitionHandler* transitionHandler;
		unsigned int currentClip;

		virtual void UpdateSkeletonInstance(AnimClip* clip_, float time_);
		void UpdateSkeletonInstance(AnimClip* clipA_, AnimClip* clipB_, float timeA_, float timeB_, float blendFactor_);

		Matrix4 GetBlendedTransform(const std::string& joint_, AnimClip* clip1_, AnimClip* clip2_, float clip1Time_, float clip2Time_, float blendFactor) const;

		unsigned int GetClipID(const std::string& clipName_) const;
		std::string GetClipName(unsigned int id_) const;
	};
}

#endif //!ANIMATOR_H