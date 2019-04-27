#include "Rigidbody.h"

#include <rttr/registration.h>

#include "PhysicsEngine.h"
#include "Math/Euler.h"
#include "Script/Script.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<PhysicsMaterial>("PhysicsMaterial")
		.constructor<float, float>()
		.property("friction", &PhysicsMaterial::friction)
		.property("bounciness", &PhysicsMaterial::bounciness)
		.method("SetFriction", &PhysicsMaterial::SetFriction)
		.method("SetBounciness", &PhysicsMaterial::SetBounciness);

	rttr::registration::class_<Rigidbody>("Rigidbody")
		.constructor()
		.constructor<float, bool, bool>()
		.method("Initialize", &Rigidbody::Initialize)
		.method("Destroy", &Rigidbody::Destroy)
		.method("PreUpdate", &Rigidbody::PreUpdate)
		.method("Update", &Rigidbody::Update)
		.method("Impulse", &Rigidbody::Impulse)
		.method("AngularImpulse", &Rigidbody::AngularImpulse)
		.method("AddCollider", &Rigidbody::AddCollider)
		.method("GetLinearVelocity", &Rigidbody::GetLinearVelocity)
		.method("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
		.method("GetAcceleration", &Rigidbody::GetAcceleration)
		.method("GetMass", &Rigidbody::GetMass)
		.method("GetMaterial", static_cast<PhysicsMaterial&(Rigidbody::*)()>(&Rigidbody::GetMaterial))
		.method("GetMaterial", static_cast<const PhysicsMaterial&(Rigidbody::*)() const>(&Rigidbody::GetMaterial))
		.method("GetFriction", &Rigidbody::GetFriction)
		.method("GetBounciness", &Rigidbody::GetBounciness)
		.method("SetLinearVelocity", &Rigidbody::SetLinearVelocity)
		.method("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
		.method("SetAcceleration", &Rigidbody::SetAcceleration)
		.method("SetMass", &Rigidbody::SetMass)
		.method("SetMaterial", &Rigidbody::SetMaterial)
		.method("SetFriction", &Rigidbody::SetFriction)
		.method("SetBounciness", &Rigidbody::SetBounciness)
		.method("SetMinLinearVelocity", static_cast<void(Rigidbody::*)(const Vector3&)>(&Rigidbody::SetMinLinearVelocity))
		.method("SetMinLinearVelocity", static_cast<void(Rigidbody::*)(float)>(&Rigidbody::SetMinLinearVelocity))
		.method("SetMaxLinearVelocity", static_cast<void(Rigidbody::*)(const Vector3&)>(&Rigidbody::SetMaxLinearVelocity))
		.method("SetMaxLinearVelocity", static_cast<void(Rigidbody::*)(float)>(&Rigidbody::SetMaxLinearVelocity))
		.method("SetLinearVelocityLimits", static_cast<void(Rigidbody::*)(const Vector3&, const Vector3&)>(&Rigidbody::SetLinearVelocityLimits))
		.method("SetLinearVelocityLimits", static_cast<void(Rigidbody::*)(float, float)>(&Rigidbody::SetLinearVelocityLimits))
		.method("SetLinearVelocityDamping", &Rigidbody::SetLinearVelocityDamping);
}
#pragma warning( pop )

Rigidbody::Rigidbody(float mass_, bool useGravity_, bool freezeRotation_) : Component(), mass(mass_), useGravity(useGravity_), externalBody(nullptr), freezeRotation(freezeRotation_), material(0.5f, 0.5f), dampingValue(0.0f){
	//Make sure that mass is positive
	_ASSERT(mass >= 0.0f);

	minLinearVelocity = Vector3::Fill(-Math::Infinity());
	maxLinearVelocity = Vector3::Fill(Math::Infinity());
}

Rigidbody::~Rigidbody(){
	#ifdef _DEBUG
	if(externalBody != nullptr || !colliders.empty() || !colliderData.empty() || !colliderAddQueue.empty() || !colliderOffsetQueue.empty()){
		Debug::LogWarning("Memory leak detected in Rigidbody!", __FILE__, __LINE__);
		Destroy();
	}
	#endif //_DEBUG
}

bool Rigidbody::Initialize(GameObject* go_){
	_ASSERT(go_ != nullptr);

	gameObject = go_;
	PhysicsEngine::RegisterRigidbody(this);
	return true;
}

void Rigidbody::Destroy(){
	std::vector<rp3d::ProxyShape*> shapes;
	auto proxShape = externalBody->getProxyShapesList();
	while(proxShape != nullptr){
		shapes.push_back(proxShape);
		proxShape = proxShape->getNext();
	}

	for(const auto& ps : shapes){
		externalBody->removeCollisionShape(ps);
	}

	for(rp3d::CollisionShape* shape : colliders){
		delete shape;
		shape = nullptr;
	}

	colliders.clear();
	colliders.shrink_to_fit();

	for(BaseCollider* data : colliderData){
		delete data;
		data = nullptr;
	}

	colliderData.clear();
	colliderData.shrink_to_fit();

	while(!colliderAddQueue.empty()){
		BaseCollider* data = colliderAddQueue.front();
		colliderAddQueue.pop();
		delete data;
		data = nullptr;
	}

	PhysicsEngine::UnregisterRigidbody(this);
	gameObject = nullptr;
}

void Rigidbody::PreUpdate(){
	while(!colliderAddQueue.empty()){
		BaseCollider* col = colliderAddQueue.front();
		colliderData.push_back(col);
		Vector3 offset = colliderOffsetQueue.front();

		rp3d::Vector3 rpOffset(offset.x, offset.y, offset.z);

		rp3d::CollisionShape* shape = nullptr;
		PizzaBox::Vector3 scale = Vector3::Zero();
		float radius = 0.0f;
		float height = 0.0f;

		switch(col->shape){
			case BaseCollider::Shape::Box:
				scale = static_cast<BoxCollider*>(col)->scale / 2.0f;
				shape = new rp3d::BoxShape(rp3d::Vector3(scale.x, scale.y, scale.z));
				externalBody->addCollisionShape(shape, rp3d::Transform(rpOffset, rp3d::Quaternion::identity()), mass);
				colliders.push_back(shape);
				break;
			case BaseCollider::Shape::Sphere:
				radius = static_cast<SphereCollider*>(col)->radius;
				shape = new rp3d::SphereShape(radius);
				externalBody->addCollisionShape(shape, rp3d::Transform(rpOffset, rp3d::Quaternion::identity()), mass);
				colliders.push_back(shape);
				break;
			case BaseCollider::Shape::Capsule:
				radius = static_cast<CapsuleCollider*>(col)->radius;
				height = static_cast<CapsuleCollider*>(col)->height;
				shape = new rp3d::CapsuleShape(radius, height);
				externalBody->addCollisionShape(shape, rp3d::Transform(rpOffset, rp3d::Quaternion::identity()), mass);
				colliders.push_back(shape);
				break;
			case BaseCollider::Shape::Convex:
				shape = static_cast<ConvexCollider*>(col)->shape;
				externalBody->addCollisionShape(shape, rp3d::Transform(rpOffset, rp3d::Quaternion::identity()), mass);
				colliders.push_back(shape);
				break;
			case BaseCollider::Shape::Concave:
				shape = static_cast<ConcaveCollider*>(col)->shape;
				externalBody->addCollisionShape(shape, rp3d::Transform(rpOffset, rp3d::Quaternion::identity()), mass);
				colliders.push_back(shape);
			default:
				break;
		}

		colliderAddQueue.pop();
		colliderOffsetQueue.pop();
	}

	//externalBody->setIsActive(GetEnable()); //Touching this breaks everything - TODO
	externalBody->setTransform(PhysicsEngine::SetupTransform(gameObject));

	externalBody->setLinearVelocity(rp3d::Vector3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
	externalBody->setAngularVelocity(rp3d::Vector3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
	externalBody->setMass(mass);
	externalBody->enableGravity(useGravity);
	externalBody->getMaterial().setBounciness(material.bounciness);
	externalBody->getMaterial().setFrictionCoefficient(material.friction);
	externalBody->setLinearDamping(dampingValue);

	externalBody->applyForceToCenterOfMass(rp3d::Vector3(linearImpulse.x, linearImpulse.y, linearImpulse.z));
	linearImpulse = Vector3::Zero();

	externalBody->applyTorque(rp3d::Vector3(angularImpulse.x, angularImpulse.y, angularImpulse.z));
	angularImpulse = Vector3::Zero();

	if(gameObject->IsStatic()){
		externalBody->setType(rp3d::BodyType::STATIC);
	}else{
		externalBody->setType(rp3d::BodyType::DYNAMIC);
	}
}

void Rigidbody::Update(float deltaTime_){
	//Make sure deltaTime is greater than 0
	_ASSERT(deltaTime_ >= 0.0f);

	const auto& newLV = externalBody->getLinearVelocity();
	const auto& newAV = externalBody->getAngularVelocity();

	linearVelocity = Vector3(newLV.x, newLV.y, newLV.z);
	angularVelocity = Vector3(newAV.x, newAV.y, newAV.z);

	ClampLinearVelocity();

	ApplyTransform(externalBody->getTransform());

	//LinearMotion(deltaTime_);
	//AngularMotion(deltaTime_);
}

void Rigidbody::Impulse(const Vector3& impulse_){
	linearImpulse += impulse_;
}

void Rigidbody::AngularImpulse(const Vector3& impulse_){
	angularImpulse += impulse_;
}

void Rigidbody::AddCollider(BaseCollider* collider_, const Vector3& offset_){
	colliderAddQueue.push(collider_);
	colliderOffsetQueue.push(offset_);
}

Vector3 Rigidbody::GetLinearVelocity() const{
	return linearVelocity;
}

Vector3 Rigidbody::GetAngularVelocity() const{
	return angularVelocity;
}

Vector3 Rigidbody::GetAcceleration() const{
	//return acceleration; ////We can't access acceleration directly with ReactPhysics
	Debug::LogWarning("Getting acceleration directly is not currently possible!", __FILE__, __LINE__);
	return Vector3::Zero();
}

float Rigidbody::GetMass() const{
	return mass;
}

void Rigidbody::SetLinearVelocity(const Vector3& vel_){
	linearVelocity = vel_;
	ClampLinearVelocity();
}

void Rigidbody::SetAngularVelocity(const Vector3& vel_){
	angularVelocity = vel_;
}

void Rigidbody::SetAcceleration(const Vector3& accel_){
	//acceleration = accel_; //We can't access acceleration directly with ReactPhysics
	Debug::LogWarning("Setting acceleration directly is not currently possible!", __FILE__, __LINE__);
}

void Rigidbody::ApplyTransform(const rp3d::Transform& transform_){
	const auto& newPos = transform_.getPosition();
	const auto& newRot = transform_.getOrientation();

	gameObject->GetTransform()->SetGlobalPosition(newPos.x, newPos.y, newPos.z);

	if(!freezeRotation){
		gameObject->GetTransform()->SetGlobalRotation(Quaternion(newRot.w, newRot.x, newRot.y, newRot.z).ToEuler());
	}
}

//void Rigidbody::LinearMotion(float deltaTime){
//	if(useGravity){
//		gameObject->GetTransform()->Translate(linearVelocity * deltaTime + 0.5f * (acceleration + PhysicsEngine::Gravity()) * deltaTime * deltaTime);
//		linearVelocity += (acceleration + PhysicsEngine::Gravity()) * deltaTime;
//	}else{
//		gameObject->GetTransform()->Translate(linearVelocity * deltaTime + 0.5f * acceleration  * deltaTime * deltaTime);
//		linearVelocity += acceleration * deltaTime;
//	}
//}
//
//void Rigidbody::AngularMotion(float deltaTime){
//	gameObject->GetTransform()->Rotate(Euler(angularVelocity) * (deltaTime / 2.0f));
//}

void Rigidbody::ClampLinearVelocity(){
	linearVelocity.x = Math::Clamp(minLinearVelocity.x, maxLinearVelocity.x, linearVelocity.x);
	linearVelocity.y = Math::Clamp(minLinearVelocity.y, maxLinearVelocity.y, linearVelocity.y);
	linearVelocity.z = Math::Clamp(minLinearVelocity.z, maxLinearVelocity.z, linearVelocity.z);
	externalBody->setLinearVelocity(rp3d::Vector3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
}