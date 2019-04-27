#ifndef COLLIDER_H
#define COLLIDER_H

#include <reactphysics3d.h>

#include "Object/Component.h"
#include "Object/GameObject.h"

namespace PizzaBox{
	class Collider : public Component{
	public:
		Collider(const Vector3& scale_, const Vector3& offset_ = Vector3::Zero(), bool isTrigger_ = false);
		~Collider();

		bool Initialize(GameObject* go_) override;
		void Destroy() override;
		
		void PreUpdate();

		static bool CheckCollision(const Collider* const c1_, const Collider* const c2_);

	private:
		friend class PhysicsEngine;

		Vector3 scale;
		bool isTrigger;
		Vector3 offset;
		rp3d::RigidBody* externalBody;
	};
}

#endif //!COLLIDER_H