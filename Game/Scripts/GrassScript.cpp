#include "GrassScript.h"

#include <Tools/Debug.h>

using namespace GamePackage;

GrassScript::GrassScript(PizzaBox::GrassSimulation* simulation_){
	simulation = simulation_;
}

GrassScript::~GrassScript(){
}

void GrassScript::OnStart(){
	Script::OnStart();

	if(simulation == nullptr){
		PizzaBox::Debug::LogWarning("GrassSimulation is null!", __FILE__, __LINE__);
		return;
	}
	
	simulation->Initialize(gameObject);
	simulation->GenerateField();
}

void GrassScript::Update(const float deltaTime_){
}

void GrassScript::OnDestroy(){
	if(simulation != nullptr){
		simulation->Destroy();
		delete simulation;
		simulation = nullptr;
	}
}

void GrassScript::SetGrassSimulation(PizzaBox::GrassSimulation* simulation_){
	simulation = simulation_;
	simulation->GenerateField();
}