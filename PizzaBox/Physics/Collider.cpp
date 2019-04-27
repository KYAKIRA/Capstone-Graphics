#include "Collider.h"

#include "AABB.h"
#include "Rigidbody.h"
#include "PhysicsEngine.h"
#include "Script/Script.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

Collider::Collider(const Vector3& scale_, const Vector3& offset_, bool isTrigger_) : Component(), scale(scale_), isTrigger(isTrigger_), offset(offset_), externalBody(nullptr){
}

Collider::~Collider(){
	#ifdef _DEBUG
	if(externalBody != nullptr){
		Debug::LogWarning("Memory leak detected in Collider!", __FILE__, __LINE__);
		Destroy();
	}
	#endif //_DEBUG
}

bool Collider::Initialize(GameObject* go_){
	_ASSERT(go_ != nullptr);

	gameObject = go_;
	PhysicsEngine::RegisterCollider(this);

	if(externalBody != nullptr){
		rp3d::Vector3 rpOffset(offset.x, offset.y, offset.z);
		externalBody->addCollisionShape(new rp3d::BoxShape(rp3d::Vector3(scale.x / 2.0f, scale.y / 2.0f, scale.z / 2.0f)), rp3d::Transform(rpOffset, rp3d::Quaternion::identity()), 0.0f);
	}

	return true;
}

void Collider::Destroy(){
	std::vector<rp3d::ProxyShape*> shapes;
	auto proxShape = externalBody->getProxyShapesList();
	while(proxShape != nullptr){
		shapes.push_back(proxShape);
		proxShape = proxShape->getNext();
	}

	for(const auto& ps : shapes){
		externalBody->removeCollisionShape(ps);
	}

	PhysicsEngine::UnregisterCollider(this);
	gameObject = nullptr;
}

void Collider::PreUpdate(){
	if(externalBody == nullptr){
		return;
	}

	externalBody->setTransform(PhysicsEngine::SetupTransform(gameObject));
}

bool Collider::CheckCollision(const Collider* const c1_, const Collider* const c2_){
	Vector3 c1Pos = c1_->gameObject->GlobalPosition() + c1_->offset;
	Vector3 c2Pos = c2_->gameObject->GlobalPosition() + c2_->offset;

	AABB c1Box = AABB(c1Pos, c1_->scale);
	AABB c2Box = AABB(c2Pos, c2_->scale);

	if(c1Box.upper.x > c2Box.lower.x && c1Box.lower.x < c2Box.upper.x){
		if(c1Box.upper.y > c2Box.lower.y && c1Box.lower.y < c2Box.upper.y){
			if(c1Box.upper.z > c2Box.lower.z && c1Box.lower.z < c2Box.upper.z){
				return true;
			}
		}
	}

	return false;
}