#include "ParticleSystem.h"

#include <algorithm>

#include <rttr/registration.h>

#include "Graphics/RenderEngine.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Object/GameObject.h"
#include "Resource/ResourceManager.h"
#include "Tools/Debug.h"
#include "Tools/Random.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<ParticleSystem>("ParticleSystem")
		.constructor<ParticleTexture*, float, float, float, float, const std::string&>()
		.method("Initialize", &ParticleSystem::Initialize)
		.method("Destroy", &ParticleSystem::Destroy)
		.method("Update", &ParticleSystem::Update)
		.method("Render", &ParticleSystem::Render)
		.method("SetVelocityChange", &ParticleSystem::SetVelocityChange)
		.method("SetRotationChange", &ParticleSystem::SetRotationChange)
		.method("SetSizeChange", &ParticleSystem::SetSizeChange)
		.method("SetSpawnRate", &ParticleSystem::SetSpawnRate)
		.method("SetInitialSpeed", &ParticleSystem::SetInitialSpeed)
		.method("SetGravityEffect", &ParticleSystem::SetGravityEffect)
		.method("SetLifeSpan", &ParticleSystem::SetLifeSpan)
		.method("SetRotation", &ParticleSystem::SetRotation)
		.method("SetScale", &ParticleSystem::SetScale)
		.method("SetDirectionRangeX", static_cast<void(ParticleSystem::*)(const Vector2&)>(&ParticleSystem::SetDirectionRangeX))
		.method("SetDirectionRangeX", static_cast<void(ParticleSystem::*)(float, float)>(&ParticleSystem::SetDirectionRangeX))
		.method("SetDirectionRangeY", static_cast<void(ParticleSystem::*)(const Vector2&)>(&ParticleSystem::SetDirectionRangeY))
		.method("SetDirectionRangeY", static_cast<void(ParticleSystem::*)(float, float)>(&ParticleSystem::SetDirectionRangeY))
		.method("SetDirectionRangeZ", static_cast<void(ParticleSystem::*)(const Vector2&)>(&ParticleSystem::SetDirectionRangeZ))
		.method("SetDirectionRangeZ", static_cast<void(ParticleSystem::*)(float, float)>(&ParticleSystem::SetDirectionRangeZ))
		.method("Translate", &ParticleSystem::Translate)
		.method("Rotate", &ParticleSystem::Rotate)
		.method("GetShader", &ParticleSystem::GetShader)
		.method("GetShaderName", &ParticleSystem::GetShaderName);
}
#pragma warning( pop )

ParticleSystem::ParticleSystem(ParticleTexture* texture_, float pps_, float speed_, float gravity_, float life_, const std::string& shaderName_) : Component(),
	shaderName(shaderName_), shader(nullptr), vao(), vbo(GL_ARRAY_BUFFER), texture(texture_), spawnTimer(0.0f), particleSpawnRate(pps_), initialSpeed(speed_), gravityScale(gravity_), lifeLength(life_), rotation(0.0f),
	scale(1.0f), dirX(Vector2(-10.0f, 10.0f)), dirY(Vector2(10.0f, 30.0f)), dirZ(Vector2(-10.0f, 10.0f)), velocityChange(Vector2(0.0f, 0.0f)), sizeChange(0.0f), rotationChange(0.0f){

	projectionMatrixID = nullptr;
	viewMatrixID = nullptr;
	modelMatrixID = nullptr;
	texOffset1ID = nullptr;
	texOffset2ID = nullptr;
	blendID = nullptr;
}

ParticleSystem::~ParticleSystem(){
	#ifdef _DEBUG
	if(projectionMatrixID != nullptr || viewMatrixID != nullptr || modelMatrixID != nullptr ||
		texOffset1ID != nullptr || texOffset2ID != nullptr || blendID != nullptr ||
		shader != nullptr || texture != nullptr || !myParticles.empty()){
		Debug::LogError("Memory leak detected in ParticleSystem!", __FILE__, __LINE__);
		Destroy();
	}
	#endif //_DEBUG
}

bool ParticleSystem::Initialize(GameObject* go_){
	//Makes sure the GameObject is a valid pointer
	_ASSERT(go_ != nullptr);

	gameObject = go_;

	shader = ResourceManager::LoadResource<Shader>(shaderName);
	if(shader == nullptr){
		Debug::LogError(shaderName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	projectionMatrixID = new Uniform(shader, "projection");
	viewMatrixID = new Uniform(shader, "viewMatrix");
	modelMatrixID = new Uniform(shader, "modelMatrix"); 
	texOffset1ID = new Uniform(shader, "texOffset1");
	texOffset2ID = new Uniform(shader, "texOffset2");
	blendID = new Uniform(shader, "texCoordInfo");

	constexpr float vertices[18] = {
		// Left bottom triangle
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		// Right top triangle
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};

	vao.Bind();
	vbo.Bind();
	vbo.SetBufferData(sizeof(vertices), &vertices, GL_STREAM_DRAW);
	vao.SetupVertexAttribute(0, 3, 3 * sizeof(float), (GLvoid*)(0));
	vbo.Unbind();
	vao.Bind();

	myParticles.clear();
	spawnTimer = 0.0f;

	RenderEngine::RegisterParticleSystem(this);
	return true;
}

void ParticleSystem::Destroy(){
	RenderEngine::UnregisterParticleSystem(this);

	if(projectionMatrixID != nullptr){
		delete projectionMatrixID;
		projectionMatrixID = nullptr;
	}

	if(viewMatrixID != nullptr){
		delete viewMatrixID;
		viewMatrixID = nullptr;
	}

	if(modelMatrixID != nullptr){
		delete modelMatrixID;
		modelMatrixID = nullptr;
	}

	if(texOffset1ID != nullptr){
		delete texOffset1ID;
		texOffset1ID = nullptr;
	}

	if(texOffset2ID != nullptr){
		delete texOffset2ID;
		texOffset2ID = nullptr;
	}

	if(blendID != nullptr){
		delete blendID;
		blendID = nullptr;
	}
	
	for(auto& p : myParticles){
		delete p;
		p = nullptr;
	}

	myParticles.clear();
	myParticles.shrink_to_fit();

	if(shader != nullptr){
		ResourceManager::UnloadResource(shaderName);
		shader = nullptr;
	}

	if(texture != nullptr){
		delete texture;
		texture = nullptr;
	}

	gameObject = nullptr;
}

void ParticleSystem::Update(){
	GenerateParticle();

	for(auto& p : myParticles){
		if(p->Update(gameObject, Time::DeltaTime(), velocityChange, rotationChange, sizeChange) == false){
			delete p;
			p = nullptr;
		}
	}

	myParticles.erase(std::remove(myParticles.begin(), myParticles.end(), nullptr), myParticles.end());
}

void ParticleSystem::Render(const Camera* camera_){
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(false);
	glDisable(GL_CULL_FACE);

	vao.Bind();
	shader->Use();
	shader->BindTexture(GL_TEXTURE0, texture->GetTexture());
	for(const auto& p : myParticles){
		shader->BindMatrix4(projectionMatrixID->id, camera_->GetProjectionMatrix());
		shader->BindMatrix4(viewMatrixID->id, camera_->GetViewMatrix());
			
		shader->BindVector2(texOffset1ID->id, p->GetTexOffset1());
		shader->BindVector2(texOffset2ID->id, p->GetTexOffset2());
		Vector2 blendInfo = Vector2(static_cast<float>(texture->GetNumOfTexture()), p->GetBlend());
		shader->BindVector2(blendID->id, blendInfo);

		Matrix4 modelMatrix = p->GetModel();
		modelMatrix *= camera_->GetGameObject()->GetRotation().ToMatrix4();

		shader->BindMatrix4(modelMatrixID->id, modelMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	
	vao.Unbind();
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(true);
}

void ParticleSystem::GenerateParticle(){
	spawnTimer += Time::DeltaTime();

	if(Math::NearZero(particleSpawnRate)){
		return; //This prevents divide by zero errors below
	}

	while(spawnTimer >= 1.0f / particleSpawnRate){
		EmitParticle();
		spawnTimer -= 1.0f / particleSpawnRate;
	}
}

void ParticleSystem::EmitParticle(){
	float dx = Random::Range(dirX.x, dirX.y);
	float dy = Random::Range(dirY.x, dirY.y);
	float dz = Random::Range(dirZ.x, dirZ.y);
	
	Vector3 velocity = Vector3(dx, dy, dz).Normalized();
	velocity *= initialSpeed;
	velocity = rotationOffset.ToMatrix4() * velocity;

	Vector3 spawnPoint = gameObject->GetPosition() + positionOffset;

	myParticles.push_back(new Particle(texture, spawnPoint, velocity, gravityScale, lifeLength, rotation, scale));
}