#ifndef TRANSITION_HANDLER_H
#define TRANSITION_HANDLER_H

#include "Transition.h"

namespace PizzaBox{
	class TransitionHandler{
	public:
		TransitionHandler(const Transition& transition_, float startTime_) : transition(transition_), startTime(startTime_){}
		
		inline std::string StartClip() const{ return transition.GetStartClip(); }
		inline std::string EndClip() const{ return transition.GetEndClip(); }
		inline float Duration() const{ return transition.GetDuration(); }
		inline float StartTime() const{ return startTime; }

	private:
		Transition transition;
		float startTime;
	};
}

#endif //!TRANSITION_HANDLER_H