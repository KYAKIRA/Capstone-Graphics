#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <map>
#include <vector>

#include "Particle.h"
#include "ParticleTexture.h"
#include "Graphics/Camera.h"
#include "Graphics/Shader.h"
#include "Graphics/LowLevel/Buffer.h"
#include "Graphics/LowLevel/Uniform.h"
#include "Graphics/LowLevel/VAO.h"
#include "Object/Component.h"

namespace PizzaBox{
	class ParticleSystem : public Component{
	public:
		ParticleSystem(ParticleTexture* texture_, float pps_, float speed_, float gravity_, float life_, const std::string& shaderName_ = "baseParticleShader");
		~ParticleSystem();

		bool Initialize(GameObject* go_) override;
		void Destroy() override;

		void Update();
		void Render(const Camera* camera_);

		inline void SetVelocityChange(const Vector2& change_){ velocityChange = change_; }
		inline void SetRotationChange(float change_){ rotationChange = change_; }
		inline void SetSizeChange(float change_){ sizeChange = change_; }
		inline void SetSpawnRate(float particlesPerSecond_){ particleSpawnRate = particlesPerSecond_; }
		inline void SetInitialSpeed(float speed_){ initialSpeed = speed_; }
		inline void SetGravityEffect(float gc_){ gravityScale = gc_; }
		inline void SetLifeSpan(float time_){ lifeLength = time_; }
		inline void SetRotation(float angle_){ rotation = angle_; }
		inline void SetScale(float scale_){ scale = scale_; }
		inline void SetDirectionRangeX(const Vector2& dir_){ dirX = dir_; }
		inline void SetDirectionRangeX(float min_, float max_){ dirX = Vector2(min_, max_); }
		inline void SetDirectionRangeY(const Vector2& dir_){ dirY = dir_; }
		inline void SetDirectionRangeY(float min_, float max_){ dirY = Vector2(min_, max_); }
		inline void SetDirectionRangeZ(const Vector2& dir_){ dirZ = dir_; }
		inline void SetDirectionRangeZ(float min_, float max_){ dirZ = Vector2(min_, max_); }
		inline void Translate(const Vector3& offset_){ positionOffset = offset_; }
		inline void Rotate(const Euler& offset_){ rotationOffset = offset_; }
		inline Shader* GetShader() const{ return shader; }
		inline std::string GetShaderName() const{ return shaderName; }

	private:
		VAO vao;
		Buffer vbo;
		Uniform* projectionMatrixID, *viewMatrixID, *modelMatrixID, *texOffset1ID, *texOffset2ID, *blendID;
		std::string shaderName;
		Shader* shader;
		ParticleTexture* texture;
		float spawnTimer;
		Vector3 positionOffset; //Move spawnPoint of particle system
		Euler rotationOffset; //Rotate entire particle system, not particles
		//List of particles with given texture
		std::vector<Particle*> myParticles;

		//Particle Properties
		float particleSpawnRate, initialSpeed, gravityScale, lifeLength, rotation, scale;
		
		//Random Direction min max
		Vector2 dirX, dirY, dirZ;

		//Change over time values
		Vector2 velocityChange;
		float sizeChange, rotationChange;

		void GenerateParticle();
		void EmitParticle();
	};
}

#endif //!PARTICLE_SYSTEM_H