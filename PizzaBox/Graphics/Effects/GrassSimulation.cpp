#include "GrassSimulation.h"

#include "Graphics/Materials/GrassMaterial.h"
#include "Graphics/Models/MeshRender.h"

using namespace PizzaBox;

GrassSimulation::GrassSimulation(const std::string& diffMap_, const std::string& specMap_, const std::string& normalMap_, int grassAmount_, float shiny_, const Vector3& sway_, float frequency_) : Component(), diffuseMapName(diffMap_), normalMapName(normalMap_), specularMapName(specMap_), shininess(shiny_),grassAmount(grassAmount_), sway(sway_), frequency(frequency_), grassField(nullptr){
}

GrassSimulation::~GrassSimulation(){
}

bool GrassSimulation::Initialize(GameObject* go_){
	gameObject = go_;
	if(gameObject == nullptr){
		Debug::LogError("gameObject could not be loaded!, gameObject was null", __FILE__, __LINE__);
		return false;
	}
	
	grassField = new GameObject();
	GenerateField();
	
	return true;
}

void GrassSimulation::GenerateField(){
	for(int i = 0; i < grassAmount; i++){
		GameObject* grass = new GameObject();
		GameObject* grass2 = new GameObject();
		grassField->AddChild(grass);
		grassField->AddChild(grass2);

		grass->GetTransform()->SetPosition(gameObject->GetPosition().x * i + 1, gameObject->GetPosition().y, gameObject->GetPosition().z);
		grass->SetScale(gameObject->GetScale());
		grass->AddComponent(new PizzaBox::MeshRender("GrassMesh", new PizzaBox::GrassMaterial(diffuseMapName, specularMapName, normalMapName, shininess, sway, frequency)));

		grass2->GetTransform()->SetPosition(gameObject->GetPosition().x * i + 1, gameObject->GetPosition().y, gameObject->GetPosition().z);
		grass2->SetScale(gameObject->GetScale());
		grass2->GetTransform()->SetRotation(0.0f, 90.0f, 0.0f);
		grass2->AddComponent(new PizzaBox::MeshRender("GrassMesh", new PizzaBox::GrassMaterial(diffuseMapName, specularMapName, normalMapName, shininess, sway, frequency)));
	}
}

void GrassSimulation::Destroy(){
	if(grassField != nullptr){
		grassField->Destroy();
		delete grassField;		
		grassField = nullptr;
	}
}