#include "Particle.h"

#include "Core/Time.h"
#include "Math/Math.h"
#include "Physics/PhysicsEngine.h"

using namespace PizzaBox;

Particle::Particle(ParticleTexture* texture_, const Vector3& position_, const Vector3& velocity_, float gravityEffect_, float lifeTime_, float rotation_, float scale_) : elapsedTime(0.0f), texture(texture_), position(position_), velocity(velocity_), gravityEffect(gravityEffect_), lifeTime(lifeTime_), rotation(rotation_), scale(scale_), blend(0.0f){
	Matrix4 positionMatrix = Matrix4::Translate(position);
	Matrix4 rotationMatrix = Matrix4::Rotate(rotation, Vector3(0.0f, 0.0f, 1.0f));
	Matrix4 scaleMatrix = Matrix4::Scale(Vector3(scale, scale, scale));

	modelMatrix = (positionMatrix * (rotationMatrix * scaleMatrix));
}

Particle::~Particle(){
}

bool Particle::Update(GameObject* gameObject_, float deltaTime_, const Vector2& acceleration_, float angularAcc_, float sizeChange_){
	velocity += acceleration_ * deltaTime_;
	rotation += angularAcc_ * deltaTime_;
	scale += sizeChange_ * deltaTime_;

	position += gameObject_->GetRotation().ToMatrix4() * (velocity * deltaTime_) + 0.5f * PhysicsEngine::Gravity() * Math::Pow(deltaTime_, 2);
	velocity.y += PhysicsEngine::Gravity().y * gravityEffect * deltaTime_; //Gravity is a change in seconds

	Matrix4 positionMatrix = Matrix4::Translate(position);
	Matrix4 rotationMatrix = Matrix4::Rotate(rotation, Vector3(0.0f, 0.0f, 1.0f));
	Matrix4 scaleMatrix = Matrix4::Scale(Vector3(scale, scale, scale));

	modelMatrix = (positionMatrix * (rotationMatrix * scaleMatrix));

	UpdateTexture();

	elapsedTime += deltaTime_;
	return elapsedTime < lifeTime;
}

void Particle::UpdateTexture(){
	float lifeFactor = elapsedTime / lifeTime;
	int stageCount = texture->GetNumOfTexture() * texture->GetNumOfTexture();
	int atLasProgression = static_cast<int>(floor(lifeFactor * stageCount));
	int index1 = atLasProgression;
	int index2 = index1 < stageCount - 1 ? index1 + 1 : index1;
	blend = static_cast<float>(atLasProgression % 1);
	SetTexOffset(texOffset1, index1);
	SetTexOffset(texOffset2, index2);
}

void Particle::SetTexOffset(Vector2& offset_, int index_){
	int column = index_ % texture->GetNumOfTexture();
	int row = index_ / texture->GetNumOfTexture();
	float newX = static_cast<float>(column) / static_cast<float>(texture->GetNumOfTexture());
	float newY = static_cast<float>(row) / static_cast<float>(texture->GetNumOfTexture());
	offset_.x = newX;
	offset_.y = newY;
}