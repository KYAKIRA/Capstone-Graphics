#ifndef PARTICLE_H
#define PARTICLE_H

#include "ParticleTexture.h"
#include "Math/Quaternion.h"
#include "Math/Vector.h"
#include "Object/GameObject.h"

namespace PizzaBox{
	class Particle{
	public:
		Particle(ParticleTexture* texture_, const Vector3& position_, const Vector3& velocity_, float gravityEffect_, float lifeTime_, float rotation_, float scale_);
		~Particle();
		
		bool Update(GameObject* gameObject_, float deltaTime_, const Vector2& acceleration_ = Vector2(0.0f, 0.0f), float angularAcc_ = 0.0f, float sizeChange_ = 0.0f);

		inline Vector3 GetPosition() const { return position; }
		inline float GetRotation() const { return rotation; }
		inline float GetScale() const { return scale; }
		inline Matrix4 GetModel() const { return modelMatrix; }
		inline ParticleTexture* GetTexture() const { return texture; }
		inline Vector2 GetTexOffset1() const { return texOffset1; }
		inline Vector2 GetTexOffset2() const { return texOffset2; }
		inline float GetBlend() const { return blend; }

	private:
		float lifeTime, gravityEffect, rotation, scale;
		Vector3 position, velocity;
		float elapsedTime;
		Matrix4 modelMatrix;
		ParticleTexture* texture;

		Vector2 texOffset1, texOffset2;
		float blend;

		void UpdateTexture();
		void SetTexOffset(Vector2& offset_, int index_);
	};
}

#endif //!PARTICLE_H