#include "DefaultScene.h"

#include <Audio/AudioListener.h>
#include <Audio/AudioSource.h>
#include <Graphics/Camera.h>
#include <Graphics/Models/MeshRender.h>
#include <Graphics/Sky/SkyBox.h>
#include <Graphics/Materials/GrassMaterial.h>
#include <Graphics/Materials/WaterMaterial.h>
#include <Graphics/Materials/ReflectiveMaterial.h>
#include <Physics/Rigidbody.h>
#include <Physics/Collider.h>

#include "../Scripts/CharacterController.h"
#include "../Scripts/TrackballControlled.h"

using namespace GamePackage;

DefaultScene::DefaultScene() : Scene(){
}

DefaultScene::~DefaultScene(){
}

bool DefaultScene::Initialize(){
	SetSky(new PizzaBox::SkyBox("LakeSkybox"));

	//Create all GameObjects for this scene
	PizzaBox::GameObject* mainCamera = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(0.0f, 0.0f, 20.0f));
	mainCamera->AddComponent(new PizzaBox::Camera(PizzaBox::ViewportRect::fullScreen, PizzaBox::Camera::RenderMode::Perspective));
	mainCamera->AddComponent(new PizzaBox::AudioListener());
	mainCamera->AddComponent(new CharacterController());
	mainCamera->AddComponent(new TrackballControlled());

	auto dirLight = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(), PizzaBox::Euler(-35.0f, 12.0f, 0.0f));
	dirLight->AddComponent(new PizzaBox::DirectionalLight(2.0f));

	PizzaBox::GameObject* cube = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(0.0f, 0.0f, -20.0f));
	cube->AddComponent(new PizzaBox::MeshRender("CubeModel", "SkullTexture"));
	cube->AddComponent(new PizzaBox::Collider(cube->GetTransform()->GlobalScale()));
	cube->AddComponent(new PizzaBox::PointLight(0.4f, PizzaBox::Color::Green));

	PizzaBox::GameObject* cubeRight = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(10.0f, 0.0f, -20.0f), PizzaBox::Euler(), PizzaBox::Vector3(1.0f, 5.0f, 5.0f));
	cubeRight->AddComponent(new PizzaBox::MeshRender("CubeModel", "MarsTexture"));
	cubeRight->AddComponent(new PizzaBox::Collider(cubeRight->GetTransform()->GlobalScale()));
	cubeRight->AddComponent(new PizzaBox::PointLight(0.4f,PizzaBox::Color::Blue));

	PizzaBox::GameObject* cubeLeft = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(-10.0f, 0.0f, -20.0f), PizzaBox::Euler(), PizzaBox::Vector3(1.0f, 5.0f, 5.0f));
	cubeLeft->AddComponent(new PizzaBox::MeshRender("CubeModel", "MarsTexture"));
	cubeLeft->AddComponent(new PizzaBox::Collider(cubeLeft->GetTransform()->GlobalScale()));
	cubeRight->AddComponent(new PizzaBox::PointLight(0.4f, PizzaBox::Color::Red));

	PizzaBox::GameObject* skull = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(10.0f, 0.0f, 10.0f));
	skull->AddComponent(new PizzaBox::MeshRender("SkullModel", "SkullTexture"));
	skull->AddComponent(new PizzaBox::Collider(cube->GetTransform()->GlobalScale()));

	PizzaBox::GameObject* floor = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(0.0f, -10.0f, 0.0f), PizzaBox::Euler(), PizzaBox::Vector3(32.0f, 0.01f, 32.0f));
	floor->AddComponent(new PizzaBox::MeshRender("CubeModel", "EarthTexture"));

	PizzaBox::GameObject* grassFloor = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(0.0f, -9.98f, 0.0f), PizzaBox::Euler(), PizzaBox::Vector3(2.0f, 2.0f, 2.0f));
	auto grass = new PizzaBox::GrassMaterial("GrassTexture","","",32.0f,PizzaBox::Vector3(0.5f,0.0f,0.0f),0.0025f);
	auto gr = new PizzaBox::MeshRender("GrassMeshMulti", grass);
	grassFloor->AddComponent(gr);
	
	PizzaBox::GameObject* grassFloor2 = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(10.0f, -9.98f, 0.0f), PizzaBox::Euler(), PizzaBox::Vector3(2.5f, 2.5f, 2.5f));
	auto grass2 = new PizzaBox::GrassMaterial("GrassTexture", "", "", 32.0f, PizzaBox::Vector3(0.2f, 0.0f, 0.0f), 0.0045f);
	auto gr2 = new PizzaBox::MeshRender("GrassMeshMulti", grass2);
	grassFloor2->AddComponent(gr2);

	PizzaBox::GameObject* baseWater = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(0.0f, -40.0f, 0.0f), PizzaBox::Euler(), PizzaBox::Vector3(140.0f, 40.0f, 140.0f));
	auto water = new PizzaBox::WaterMaterial("WaterTexture", "", "", 32.0f, 256.0f);
	water->SetWaveParamaters(PizzaBox::Vector4(2.0f, 4.0f, 4.0f, 2.0f), PizzaBox::Vector4(0.8f, 0.2f, 0.2f, 0.2f), PizzaBox::Vector4(0.4f, 0.4f, 0.4f, 0.2f));
	water->SetFlowDirection(PizzaBox::Vector2(0.0f, 1.0f));
	auto mr = new PizzaBox::MeshRender("WaterMesh", water);
	mr->SetCastsShadows(false);
	baseWater->AddComponent(mr);

	PizzaBox::GameObject* sphereReflection = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(5.0f, 0.0f, 0.0f), PizzaBox::Euler(), PizzaBox::Vector3(1.0f, 1.0f, 1.0f));
	auto rf0 = new PizzaBox::ReflectiveMaterial(true);
	sphereReflection->AddComponent(new PizzaBox::MeshRender("SphereModel", rf0));

	PizzaBox::GameObject* sphereRefractionAir = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(5.0f, 0.0f, 5.0f), PizzaBox::Euler(), PizzaBox::Vector3(1.0f, 1.0f, 1.0f));
	auto rf1 = new PizzaBox::ReflectiveMaterial(false,PizzaBox::ReflectiveMaterial::air);
	sphereRefractionAir->AddComponent(new PizzaBox::MeshRender("SphereModel", rf1));

	PizzaBox::GameObject* sphereRefractionIce = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(5.0f, 0.0f, 10.0f), PizzaBox::Euler(), PizzaBox::Vector3(1.0f, 1.0f, 1.0f));
	auto rf2 = new PizzaBox::ReflectiveMaterial(false, PizzaBox::ReflectiveMaterial::ice);
	sphereRefractionIce->AddComponent(new PizzaBox::MeshRender("SphereModel", rf2));

	PizzaBox::GameObject* sphereRefractionWater = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(5.0f, 0.0f, 15.0f), PizzaBox::Euler(), PizzaBox::Vector3(1.0f, 1.0f, 1.0f));
	auto rf3 = new PizzaBox::ReflectiveMaterial(false, PizzaBox::ReflectiveMaterial::water);
	sphereRefractionWater->AddComponent(new PizzaBox::MeshRender("SphereModel", rf3));

	PizzaBox::GameObject* sphereRefractionGlass= CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(5.0f, 0.0f, 20.0f), PizzaBox::Euler(), PizzaBox::Vector3(1.0f, 1.0f, 1.0f));
	auto rf4 = new PizzaBox::ReflectiveMaterial(false, PizzaBox::ReflectiveMaterial::glass);
	sphereRefractionGlass->AddComponent(new PizzaBox::MeshRender("SphereModel", rf4));

	PizzaBox::GameObject* sphereRefractionDiamond = CreateObject<PizzaBox::GameObject>(PizzaBox::Vector3(5.0f, 0.0f, 25.0f), PizzaBox::Euler(), PizzaBox::Vector3(1.0f, 1.0f, 1.0f));
	auto rf5 = new PizzaBox::ReflectiveMaterial(false, PizzaBox::ReflectiveMaterial::diamond);
	sphereRefractionDiamond->AddComponent(new PizzaBox::MeshRender("SphereModel", rf5));

	return true;
}

void DefaultScene::Destroy(){
	Scene::Destroy();
}