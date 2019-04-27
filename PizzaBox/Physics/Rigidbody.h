#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <reactphysics3d.h>

#include "ColliderTypes.h"
#include "Math/Math.h"
#include "Object/Component.h"
#include "Object/GameObject.h"

namespace PizzaBox{
	struct PhysicsMaterial{
		PhysicsMaterial(float f_, float b_) : friction(f_), bounciness(b_){
			SetFriction(f_);
			SetBounciness(b_);
		}

		float friction;
		float bounciness;

		void SetFriction(float f_){
			friction = Math::Clamp(0.0f, 1.0f, f_);
		}

		void SetBounciness(float b_){
			//ReactPhysics forces us to restrict bounciness
			bounciness = Math::Clamp(0.0f, 1.0f, b_);
		}
	};

	class Rigidbody : public Component{
	public:
		explicit Rigidbody(float mass_ = 1.0f, bool useGravity_ = true, bool freezeRotation_ = false);
		virtual ~Rigidbody() override;

		virtual bool Initialize(GameObject* go_) override;
		virtual void Destroy() override;

		void PreUpdate();
		void Update(float deltaTime_);
		void Impulse(const Vector3& impulse_);
		void AngularImpulse(const Vector3& impulse_);

		void AddCollider(BaseCollider* collider_, const Vector3& offset_ = Vector3::Zero());

		//Getters
		Vector3 GetLinearVelocity() const;
		Vector3 GetAngularVelocity() const;
		Vector3 GetAcceleration() const;
		float GetMass() const;
		const PhysicsMaterial& GetMaterial() const{ return material; }
		PhysicsMaterial& GetMaterial(){ return material; }
		float GetFriction() const{ return material.friction; }
		float GetBounciness() const{ return material.bounciness; }

		//Setters
		void SetLinearVelocity(const Vector3& vel_);
		void SetAngularVelocity(const Vector3& vel_);
		void SetAcceleration(const Vector3& accel_);
		inline void SetMass(float mass_){ mass = mass_; }
		inline void SetMaterial(const PhysicsMaterial& material_){ material = material_; }
		inline void SetFriction(float friction_){ material.SetFriction(friction_); }
		inline void SetBounciness(float bounce_){ material.SetBounciness(bounce_); }

		inline void SetMinLinearVelocity(const Vector3& v_){ minLinearVelocity = v_; }
		inline void SetMinLinearVelocity(float f_){ minLinearVelocity = Vector3::Fill(f_); }
		inline void SetMaxLinearVelocity(const Vector3& v_){ maxLinearVelocity = v_; }
		inline void SetMaxLinearVelocity(float f_){ maxLinearVelocity = Vector3::Fill(f_); }

		inline void SetLinearVelocityLimits(const Vector3& min_, const Vector3& max_){
			SetMinLinearVelocity(min_);
			SetMaxLinearVelocity(max_);
		}

		inline void SetLinearVelocityLimits(float min_, float max_){
			SetMinLinearVelocity(min_);
			SetMaxLinearVelocity(max_);
		}

		inline void SetLinearVelocityDamping(float damping_){
			dampingValue = Math::Clamp(0.0f, 1.0f, damping_);
		}

	private:
		friend class PhysicsEngine;

		//Vector3 acceleration, angularAcceleration; //We can't access acceleration directly through ReactPhysics
		Vector3 linearVelocity, angularVelocity;
		float mass;
		bool useGravity;
		bool freezeRotation;
		PhysicsMaterial material;
		rp3d::RigidBody* externalBody;
		std::vector<rp3d::CollisionShape*> colliders;
		std::vector<BaseCollider*> colliderData;

		std::queue<BaseCollider*> colliderAddQueue;
		std::queue<Vector3> colliderOffsetQueue;

		Vector3 linearImpulse;
		Vector3 angularImpulse;

		Vector3 minLinearVelocity;
		Vector3 maxLinearVelocity;
		float dampingValue;

		void ApplyTransform(const rp3d::Transform& transform_);
		[[deprecated("This won't work correctly with ReactPhysics")]] void LinearMotion(float deltaTime_);
		[[deprecated("This won't work correctly with ReactPhysics")]] void AngularMotion(float deltaTime_);

		void ClampLinearVelocity();
	};
}

#endif //!RIGIDBODY_H