#include "PhysicsEngine.h"

#include <algorithm>

#include <collision/ContactManifold.h>
#include <constraint/ContactPoint.h>

#include "Script/Script.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

Vector3 PhysicsEngine::gravity(0.0f, -100.0f, 0.0f);
std::vector<Rigidbody*> PhysicsEngine::rbs;
std::vector<Collider*> PhysicsEngine::cols;
rp3d::DynamicsWorld* PhysicsEngine::world = nullptr;
float PhysicsEngine::updatesPerSecond = 120.0f;
float PhysicsEngine::timeAccumulator = 0.0f;
std::vector<CollisionPair> PhysicsEngine::currentCollisions;

bool PhysicsEngine::Initialize(){
	world = new rp3d::DynamicsWorld(rp3d::Vector3(gravity.x, gravity.y, gravity.z));
	world->setIsGratityEnabled(true);
	world->enableSleeping(true);

	timeAccumulator = 0.0f;

	return true;
}

void PhysicsEngine::Destroy(){
	rbs.clear();
	rbs.shrink_to_fit();

	cols.clear();
	cols.shrink_to_fit();

	delete world;
	world = nullptr;

	currentCollisions.clear();
}

void PhysicsEngine::RegisterRigidbody(Rigidbody* rb_){
	_ASSERT(rb_ != nullptr);
	rbs.push_back(rb_);

	rb_->externalBody = world->createRigidBody(SetupTransform(rb_->GetGameObject()));
	rb_->externalBody->enableGravity(rb_->useGravity);
	//rb_->externalBody->setIsActive(rb_->GetEnable()); //Don't touch this or the entire physics library breaks - TODO, figure that out
	rb_->externalBody->setMass(rb_->GetMass());

	if(rb_->GetGameObject()->IsStatic()){
		rb_->externalBody->setType(rp3d::BodyType::STATIC);
	}else{
		rb_->externalBody->setType(rp3d::BodyType::DYNAMIC);
	}

	rb_->externalBody->getMaterial().setBounciness(rb_->material.bounciness);
	rb_->externalBody->getMaterial().setFrictionCoefficient(rb_->material.friction);
}

void PhysicsEngine::RegisterCollider(Collider* col_){
	_ASSERT(col_ != nullptr);
	cols.push_back(col_);

	if(!col_->isTrigger){
		col_->externalBody = world->createRigidBody(SetupTransform(col_->GetGameObject()));
		col_->externalBody->enableGravity(false);
		//col_->externalBody->setIsActive(true); //Don't touch this or the entire physics library breaks - TODO
		col_->externalBody->setMass(0.0f);
		col_->externalBody->setType(rp3d::BodyType::STATIC);
		col_->externalBody->getMaterial().setBounciness(0.0f);
		col_->externalBody->getMaterial().setFrictionCoefficient(0.0f);
	}
	
	//if(col_->isTrigger){
	//	col_->externalBody->setIsActive(false); //TODO - etc
	//}
}

void PhysicsEngine::UnregisterRigidbody(Rigidbody* rb_){
	_ASSERT(rb_ != nullptr);
	rbs.erase(std::remove(rbs.begin(), rbs.end(), rb_), rbs.end());

	world->destroyRigidBody(rb_->externalBody);
	rb_->externalBody = nullptr;

	for(auto& c : currentCollisions){
		if(c.obj1 == rb_->GetGameObject() || c.obj2 == rb_->GetGameObject()){
			c.obj1 = nullptr;
			c.obj2 = nullptr;
		}
	}

	currentCollisions.erase(std::remove_if(currentCollisions.begin(), currentCollisions.end(), [](CollisionPair& cp){
		return (cp.obj1 == nullptr) || (cp.obj2 == nullptr);
	}), currentCollisions.end());
}

void PhysicsEngine::UnregisterCollider(Collider* col_){
	_ASSERT(col_ != nullptr);
	cols.erase(std::remove(cols.begin(), cols.end(), col_), cols.end());

	if(col_->externalBody != nullptr){
		world->destroyRigidBody(col_->externalBody);
		col_->externalBody = nullptr;
	}

	for(auto& c : currentCollisions){
		if(c.obj1 == col_->GetGameObject() || c.obj2 == col_->GetGameObject()){
			c.obj1 = nullptr;
			c.obj2 = nullptr;
		}
	}

	currentCollisions.erase(std::remove_if(currentCollisions.begin(), currentCollisions.end(), [](CollisionPair& cp){
		return (cp.obj1 == nullptr) || (cp.obj2 == nullptr);
	}), currentCollisions.end());
}

void PhysicsEngine::Update(const float deltaTime_){
	//Make sure that deltaTime is positive
	_ASSERT(deltaTime_ >= 0.0f);

	for(Rigidbody* rb : rbs){
		rb->PreUpdate();
	}

	for(Collider* col : cols){
		col->PreUpdate();
	}

	timeAccumulator += deltaTime_;
	while(timeAccumulator >= 1.0f / updatesPerSecond){
		world->update(1.0f / updatesPerSecond);
		timeAccumulator -= 1.0f / updatesPerSecond;
	}

	for(Rigidbody* rb : rbs){
		rb->Update(deltaTime_);
	}

	//We haven't checked any collision pairs this frame so set everything to 
	for(auto& c : currentCollisions){
		c.checkedThisFrame = false;
	}

	for(const auto& collisions : world->getContactsList()){
		auto b1 = collisions->getBody1();
		auto b2 = collisions->getBody2();

		GameObject* go1 = nullptr;
		GameObject* go2 = nullptr;

		for(Rigidbody* rb : rbs){
			if(rb->externalBody == b1){
				go1 = rb->GetGameObject();
			}else if(rb->externalBody == b2){
				go2 = rb->GetGameObject();
			}

			if(go1 && go2){
				break;
			}
		}

		for(Collider* col : cols){
			if(col->externalBody == b1){
				go1 = col->GetGameObject();
			}else if(col->externalBody == b2){
				go2 = col->GetGameObject();
			}

			if(go1 && go2){
				break;
			}
		}

		if(go1 == nullptr || go2 == nullptr){
			Debug::LogError("Unregistered components are somehow colliding!", __FILE__, __LINE__);
			continue;
		}

		bool foundCollision = false;
		for(auto& v : currentCollisions){
			if(v == CollisionPair(go1, go2)){
				//This collision is still happening
				foundCollision = true;
				UpdateCollisionPair(go1, go2, collisions);
			}
		}

		if(!foundCollision){
			AddCollisionPair(go1, go2, collisions);
		}
	}

	for(const auto& c : currentCollisions){
		if(!c.checkedThisFrame){
			RemoveCollisionPair(c.obj1, c.obj2);
		}
	}
}

std::vector<RaycastInfo> PhysicsEngine::Raycast(const Vector3& start_, const Vector3& end_){
	std::vector<RaycastInfo> infos;

	rp3d::Ray ray = rp3d::Ray(rp3d::Vector3(start_.x, start_.y, start_.z), rp3d::Vector3(end_.x, end_.y, end_.z));
	rp3d::RaycastInfo in;

	for(Rigidbody* rb : rbs){
		if(!rb->GetEnable()){
			continue;
		}

		if(rb->externalBody->raycast(ray, in)){
			infos.push_back(PizzaBox::RaycastInfo(
				Vector3(in.worldPoint.x, in.worldPoint.y, in.worldPoint.z),
				Vector3(in.worldNormal.x, in.worldNormal.y, in.worldNormal.z),
				in.hitFraction, rb->GetGameObject()
			));
		}
	}

	for(Collider* col : cols){
		if(!col->GetEnable()){
			continue;
		}

		if(col->externalBody->raycast(ray, in)){
			infos.push_back(PizzaBox::RaycastInfo(
				Vector3(in.worldPoint.x, in.worldPoint.y, in.worldPoint.z),
				Vector3(in.worldNormal.x, in.worldNormal.y, in.worldNormal.z),
				in.hitFraction, col->GetGameObject()
			));
		}
	}

	return infos;
}

void PhysicsEngine::SetGravity(const Vector3& vec){
	gravity = vec;
	auto rpGravity = rp3d::Vector3(gravity.x, gravity.y, gravity.z);
	world->setGravity(rpGravity);
}

rp3d::Transform PhysicsEngine::SetupTransform(const GameObject* go_){
	const Vector3& objectPos = go_->GetTransform()->GlobalPosition();
	const Quaternion& objectRot = go_->GetTransform()->GlobalRotationQuat();

	rp3d::Vector3 pos(objectPos.x, objectPos.y, objectPos.z);
	rp3d::Quaternion rot(objectRot.x, objectRot.y, objectRot.z, objectRot.w);
	return rp3d::Transform(pos, rot);
}

void PhysicsEngine::AddCollisionPair(GameObject* go1_, GameObject* go2_, const rp3d::ContactManifold* contactInfo_){
	CollisionPair pair = CollisionPair(go1_, go2_);

	for(const auto& cp : currentCollisions){
		if(cp == pair){
			return;
		}
	}

	pair.checkedThisFrame = true;
	currentCollisions.push_back(pair);

	Vector3 normal = Vector3::Zero();

	auto contactPoint = contactInfo_->getContactPoints();
	if(contactPoint != nullptr){
		auto rp3dNormal = contactPoint->getNormal();
		normal = Vector3(rp3dNormal.x, rp3dNormal.y, rp3dNormal.z);
	}

	auto scripts = go1_->GetComponents<Script>();
	for(Script* s : scripts){
		s->RegisterCollisionEnter(CollisionInfo(go2_, normal));
	}

	scripts = go2_->GetComponents<Script>();
	for(Script* s : scripts){
		s->RegisterCollisionEnter(CollisionInfo(go1_, normal));
	}
}

void PhysicsEngine::UpdateCollisionPair(GameObject* go1_, GameObject* go2_, const rp3d::ContactManifold* contactInfo_){
	CollisionPair pair = CollisionPair(go1_, go2_);

	for(auto& cp : currentCollisions){
		if(cp == pair){
			cp.checkedThisFrame = true;

			Vector3 normal = Vector3::Zero();

			auto contactPoint = contactInfo_->getContactPoints();
			if(contactPoint != nullptr){
				auto rp3dNormal = contactPoint->getNormal();
				normal = Vector3(rp3dNormal.x, rp3dNormal.y, rp3dNormal.z);
			}

			auto scripts = go1_->GetComponents<Script>();
			for(Script* s : scripts){
				s->RegisterCollisionStay(CollisionInfo(go2_, normal));
			}

			scripts = go2_->GetComponents<Script>();
			for(Script* s : scripts){
				s->RegisterCollisionStay(CollisionInfo(go1_, normal));
			}
		}
	}
}

void PhysicsEngine::RemoveCollisionPair(GameObject* go1_, GameObject* go2_){
	CollisionPair pair = CollisionPair(go1_, go2_);
	bool handleCollisionExit = false;

	for(const auto& cp : currentCollisions){
		if(cp == pair){
			handleCollisionExit = true;
			break;
		}
	}

	currentCollisions.erase(std::remove(currentCollisions.begin(), currentCollisions.end(), pair), currentCollisions.end());

	if(handleCollisionExit){
		auto scripts = go1_->GetComponents<Script>();
		for(Script* s : scripts){
			s->RegisterCollisionExit(go2_);
		}

		scripts = go2_->GetComponents<Script>();
		for(Script* s : scripts){
			s->RegisterCollisionExit(go1_);
		}
	}
}