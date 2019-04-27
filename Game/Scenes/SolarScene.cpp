#include "SolarScene.h"

#include <Audio/AudioListener.h>
#include <Graphics/Camera.h>
#include <Graphics/Models/MeshRender.h>
#include <Graphics/Lighting/PointLight.h>
#include <Graphics/Sky/SkyBox.h>
#include <Graphics/Text/TextRender.h>
#include <Physics/Rigidbody.h>

#include "../Scripts/CharacterController.h"
#include "../Scripts/SolarSystem.h"
#include "../Scripts/TrackballControlled.h"

using namespace GamePackage;

constexpr int outerPlanets = 40; //30 looks ok for showing people
constexpr int innerPlanets = 35; //30 looks ok for showing people

SolarScene::SolarScene() : Scene(){
}

SolarScene::~SolarScene(){
}

bool SolarScene::Initialize(){ 
	SetSky(new PizzaBox::SkyBox("StarsSkybox"));

	PizzaBox::GameObject* mainCamera = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(0.0f, 0.0f, 130.0f)); //z can see neptune 560.0f
	mainCamera->AddComponent(new PizzaBox::Camera(PizzaBox::ViewportRect::fullScreen, PizzaBox::Camera::RenderMode::Perspective));
	mainCamera->AddComponent(new PizzaBox::AudioListener());
	mainCamera->AddComponent(new CharacterController());  
	
	//Solar Controller - controls the gravity
	PizzaBox::GameObject* solarController = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(0.0f, 0.0f, 0.0f));
	auto sc = new SolarSystem();
	solarController->AddComponent(sc);
	
	//Sun
	PizzaBox::GameObject* sun = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(0.0f, 0.0f, 0.0f));
	sc->planets.push_back(sun);
	sun->AddComponent(new PizzaBox::PointLight(50.0f));
	auto rb = new PizzaBox::Rigidbody(1.989e+20f, false);
	rb->AngularImpulse(PizzaBox::Vector3(0.0f, 50.0f, 0.0f));
	sun->AddComponent(rb);
		// Model
		PizzaBox::GameObject* sunModel = CreateObject<PizzaBox::GameObject>(sun, PizzaBox::Vector3(0.0f, 0.0f, 0.0f), PizzaBox::Euler(-90.0f, 0.0f, 0.0f), PizzaBox::Vector3(5.0f, 5.0f, 5.0f));
		sunModel->AddComponent(new PizzaBox::MeshRender("SphereModel", "SunTexture")); 
		 
	//Mercury
	PizzaBox::GameObject* mercury = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(5.91f, 0.0f, 0.0f)); 
	sc->planets.push_back(mercury); 
	rb = new PizzaBox::Rigidbody(3.285e+13f, false);
	rb->AngularImpulse(PizzaBox::Vector3(0.0f, 50.0f, 0.0f));
	rb->SetLinearVelocity(mercury->GetTransform()->GetForward() * 0.409212f * (innerPlanets - 1));
	mercury->AddComponent(rb);
		// Model
		PizzaBox::GameObject* mercuryModel = CreateObject<PizzaBox::GameObject>(mercury, PizzaBox::Vector3(0.0f, 0.0f, 0.0f), PizzaBox::Euler(-90.0f, 0.0f, 0.0f), PizzaBox::Vector3(0.6f, 0.6f, 0.6f));
		mercuryModel->AddComponent(new PizzaBox::MeshRender("SphereModel", "MercuryTexture")); 
		 
	//Venus
	PizzaBox::GameObject* venus = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(10.82f, 0.0f, 0.0f));
	sc->planets.push_back(venus); 
	rb = new PizzaBox::Rigidbody(4.867e+14f, false);
	rb->AngularImpulse(PizzaBox::Vector3(0.0f, 10.0f, 0.0f));
	rb->SetLinearVelocity(venus->GetTransform()->GetForward() * 0.3025848f * innerPlanets);
	venus->AddComponent(rb);
		// Model
		PizzaBox::GameObject* venusModel = CreateObject<PizzaBox::GameObject>(venus, PizzaBox::Vector3(0.0f, 0.0f, 0.0f), PizzaBox::Euler(-90.0f, 0.0f, 0.0f), PizzaBox::Vector3(1.0f, 1.0f, 1.0f));
		venusModel->AddComponent(new PizzaBox::MeshRender("SphereModel", "VenusTexture")); 

	//Earth
	PizzaBox::GameObject* earth = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(14.96f, 0.0f, 0.0f)); 
	sc->planets.push_back(earth);  
	rb = new PizzaBox::Rigidbody(5.972e+14f, false);
	rb->AngularImpulse(PizzaBox::Vector3(0.0f, 50.0f, 0.0f));
	rb->SetLinearVelocity(earth->GetTransform()->GetForward() * 0.2592f * innerPlanets);
	earth->AddComponent(rb);
		// Model
		PizzaBox::GameObject* earthModel = CreateObject<PizzaBox::GameObject>(earth, PizzaBox::Vector3(0.0f, 0.0f, 0.0f), PizzaBox::Euler(-90.0f, 0.0f, 0.0f), PizzaBox::Vector3(1.2f, 1.2f, 1.2f));
		earthModel->AddComponent(new PizzaBox::MeshRender("SphereModel", "EarthTexture"));
	
	//Mars
	PizzaBox::GameObject* mars = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(22.79f, 0.0f, 0.0f)); 
	sc->planets.push_back(mars); 
	rb = new PizzaBox::Rigidbody(5.972e+14f, false);
	rb->AngularImpulse(PizzaBox::Vector3(0.0f, 50.0f, 0.0f));
	rb->SetLinearVelocity(mars->GetTransform()->GetForward() * 0.2084904f * innerPlanets);
	mars->AddComponent(rb);
		// Model
		PizzaBox::GameObject* marsModel = CreateObject<PizzaBox::GameObject>(mars, PizzaBox::Vector3(0.0f, 0.0f, 0.0f), PizzaBox::Euler(-90.0f, 0.0f, 0.0f), PizzaBox::Vector3(0.8f, 0.8f, 0.8f));
		marsModel->AddComponent(new PizzaBox::MeshRender("SphereModel", "MarsTexture"));  

	//Jupiter
	PizzaBox::GameObject* jupiter = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(77.85f, 0.0f, 0.0f));  
	sc->planets.push_back(jupiter); 
	rb = new PizzaBox::Rigidbody(1.898e+17f, false);
	rb->AngularImpulse(PizzaBox::Vector3(0.0f, 100.0f, 0.0f));
	rb->SetLinearVelocity(jupiter->GetTransform()->GetForward() * 0.1129224f * outerPlanets);
	jupiter->AddComponent(rb);
		// Model
		PizzaBox::GameObject* jupiterModel = CreateObject<PizzaBox::GameObject>(jupiter, PizzaBox::Vector3(0.0f, 0.0f, 0.0f), PizzaBox::Euler(-90.0f, 0.0f, 0.0f), PizzaBox::Vector3(3.5f, 3.5f, 3.5f));
		jupiterModel->AddComponent(new PizzaBox::MeshRender("SphereModel", "JupiterTexture")); 

	//Saturn
	PizzaBox::GameObject* saturn = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(142.9f, 0.0f, 0.0f)); 
	sc->planets.push_back(saturn); 
	rb = new PizzaBox::Rigidbody(5.683e+16f, false);
	rb->AngularImpulse(PizzaBox::Vector3(0.0f, 70.0f, 0.0f));
	rb->SetLinearVelocity(saturn->GetTransform()->GetForward() * 0.0835704f * outerPlanets);
	saturn->AddComponent(rb);
		// Model
		PizzaBox::GameObject* saturnModel = CreateObject<PizzaBox::GameObject>(saturn, PizzaBox::Vector3(0.0f, 0.0f, 0.0f), PizzaBox::Euler(-90.0f, 0.0f, 0.0f), PizzaBox::Vector3(3.2f, 3.2f, 3.2f));
		saturnModel->AddComponent(new PizzaBox::MeshRender("SphereModel", "SaturnTexture")); 

	//Uranus
	PizzaBox::GameObject* uranus = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(287.1f, 0.0f, 0.0f)); 
	rb = new PizzaBox::Rigidbody(8.681e+15f, false);
	rb->AngularImpulse(PizzaBox::Vector3(0.0f, 60.0f, 0.0f));
	rb->SetLinearVelocity(uranus->GetTransform()->GetForward() * 0.0590568f * outerPlanets);
	uranus->AddComponent(rb);
		// Model
		PizzaBox::GameObject* uranusModel = CreateObject<PizzaBox::GameObject>(uranus, PizzaBox::Vector3(0.0f, 0.0f, 0.0f), PizzaBox::Euler(-90.0f, 0.0f, 0.0f), PizzaBox::Vector3(3.0f, 3.0f, 3.0f)); 
		uranusModel->AddComponent(new PizzaBox::MeshRender("SphereModel", "UranusTexture")); 

	//Neptune
	PizzaBox::GameObject* neptune = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(449.5f, 0.0f, 0.0f)); 
	rb = new PizzaBox::Rigidbody(1.024e+16f, false);
	rb->AngularImpulse(PizzaBox::Vector3(0.0f, 65.0f, 0.0f));
	rb->SetLinearVelocity(neptune->GetTransform()->GetForward() * 0.047328f * outerPlanets);
	neptune->AddComponent(rb);
		// Model
		PizzaBox::GameObject* neptuneModel = CreateObject<PizzaBox::GameObject>(neptune, PizzaBox::Vector3(0.0f, 0.0f, 0.0f), PizzaBox::Euler(-90.0f, 0.0f, 0.0f), PizzaBox::Vector3(3.0f, 3.0f, 3.0f));
		neptuneModel->AddComponent(new PizzaBox::MeshRender("SphereModel", "NeptuneTexture")); 
   
	return true;
}

void SolarScene::Destroy(){
	Scene::Destroy();
}