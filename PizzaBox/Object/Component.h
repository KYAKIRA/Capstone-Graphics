#ifndef COMPONENT_H
#define COMPONENT_H

namespace PizzaBox{
	class Component{
	public:
		Component() : gameObject(nullptr), enabled(true){};
		virtual ~Component(){};

		virtual bool Initialize(class GameObject* go_) = 0;
		virtual void Destroy() = 0;

		bool GetEnable() const{
			return enabled;
		}

		GameObject* GetGameObject() const{
			return gameObject;
		}

		void SetEnable(const bool enable_){
			enabled = enable_;
		}
	protected:
		class GameObject* gameObject; //The GameObject that this component is attached to
		bool enabled; //Is this component enabled?
	};
}

#endif //!COMPONENT_H