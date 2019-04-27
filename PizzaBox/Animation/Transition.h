#ifndef TRANSITION_H
#define TRANSITION_H

#include <string>

namespace PizzaBox{
	class Transition{
	public:
		Transition(const std::string& startClip_, const std::string& endClip_, float duration_) : startClip(startClip_), endClip(endClip_), duration(duration_){
		}
		
		inline std::string GetStartClip() const{ return startClip; }
		inline std::string GetEndClip() const{ return endClip; }
		inline float GetDuration() const{ return duration; }
		
	private:
		std::string startClip;
		std::string endClip;
		float duration;
	};
}

#endif //!TRANSITION_H