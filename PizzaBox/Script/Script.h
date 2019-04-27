#ifndef SCRIPT_H
#define SCRIPT_H

#include <queue>

#include "Object/Component.h"
#include "Object/GameObject.h"

namespace PizzaBox{
	enum class UIEvent{
		OnClick,
		OnRelease,
	};

	struct CollisionInfo{
		CollisionInfo(GameObject* other_, const Vector3& normal_) : other(other_), normal(normal_){}

		GameObject* other;
		Vector3 normal;
	};
	
	class Script : public Component{
	public:
		Script();
		virtual ~Script();

		bool Initialize(GameObject* go_) override;
		void Destroy() override;

		bool HasStarted();
		
		virtual void OnStart();
		virtual void Update(const float deltaTime_) = 0;
		virtual void OnDestroy() = 0;

		virtual void OnCollision(const CollisionInfo& info_);
		virtual void OnCollisionStay(const CollisionInfo& info_);
		virtual void OnCollisionExit(GameObject* other_);
		
		virtual void OnUI(UIEvent event_);

		void RegisterCollisionEnter(CollisionInfo& info_);
		void RegisterCollisionStay(CollisionInfo& info_);
		void RegisterCollisionExit(GameObject* other_);

	private:
		//This allows the ScriptManager to access private member variables and functions of this
		friend class ScriptManager;

		bool hasStarted;
		std::queue<CollisionInfo> collisionEnterQueue;
		std::queue<CollisionInfo> collisionStayQueue;
		std::queue<GameObject*> collisionExitQueue;
	};
}

#endif //!SCRIPT_H