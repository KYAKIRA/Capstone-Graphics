#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <reactphysics3d.h>

#include "Collider.h"
#include "Rigidbody.h"

namespace PizzaBox{
	struct CollisionPair{
		CollisionPair(GameObject* go1_, GameObject* go2_){
			obj1 = go1_;
			obj2 = go2_;
			checkedThisFrame = false;
		}

		inline bool operator ==(const CollisionPair& rhs_) const{
			return ((rhs_.obj1 == obj1 && rhs_.obj2 == obj2) || (rhs_.obj1 == obj2 && rhs_.obj2 == obj1));
		}

		inline bool operator !=(const CollisionPair& rhs_) const{
			return !(*this == rhs_);
		}

		GameObject* obj1;
		GameObject* obj2;
		Vector3 contactNormal;
		bool checkedThisFrame;
	};

	struct RaycastInfo{
		RaycastInfo(const Vector3& point_ = Vector3(), const Vector3& normal_ = Vector3(), float hitFraction_ = 0.0f, GameObject* other_ = nullptr) : point(point_), normal(normal_), hitFraction(hitFraction_), other(other_){}

		Vector3 point;
		Vector3 normal;
		float hitFraction;
		GameObject* other;
	};

	class PhysicsEngine{
	public:
		static bool Initialize();
		static void Destroy();

		static void RegisterRigidbody(Rigidbody* rb_);
		static void RegisterCollider(Collider* col_);

		static void UnregisterRigidbody(Rigidbody* rb_);
		static void UnregisterCollider(Collider* col_);
		
		static void Update(float deltaTime_);
		static std::vector<RaycastInfo> Raycast(const Vector3& start_, const Vector3& end_);

		static Vector3 Gravity(){ return gravity; }
		static void SetGravity(const Vector3& vec);
		static rp3d::Transform SetupTransform(const GameObject* go_);

		//Delete unwanted compiler-generated constructors, destructors and assignment operators
		PhysicsEngine() = delete;
		PhysicsEngine(const PhysicsEngine&) = delete;
		PhysicsEngine(PhysicsEngine&&) = delete;
		PhysicsEngine& operator=(const PhysicsEngine&) = delete;
		PhysicsEngine& operator=(PhysicsEngine&&) = delete;
		~PhysicsEngine() = delete;

	private:
		static Vector3 gravity;
		static std::vector<Rigidbody*> rbs;
		static std::vector<Collider*> cols;
		static rp3d::DynamicsWorld* world;
		static float updatesPerSecond;
		static float timeAccumulator;

		static std::vector<CollisionPair> currentCollisions;

		static void AddCollisionPair(GameObject* go1_, GameObject* go2_, const rp3d::ContactManifold* contactInfo_);
		static void UpdateCollisionPair(GameObject* go1_, GameObject* go2_, const rp3d::ContactManifold* contactInfo_);
		static void RemoveCollisionPair(GameObject* go1_, GameObject* go2_);
	};
}

#endif //!PHYSICS_ENGINE_H