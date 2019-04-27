#include "CharacterController.h"

#include <Core/GameManager.h>
#include <Core/Time.h>
#include <Graphics/Models/MeshRender.h>
#include <Input/InputManager.h>
#include <Object/GameObject.h>
#include <Tools/Debug.h>

using namespace GamePackage;

CharacterController::CharacterController() : toggle(false){
}

void CharacterController::OnStart(){
	Script::OnStart();
	toggle = false;
}

void CharacterController::Update(const float deltaTime_){
	PizzaBox::Vector3 currentPosition = gameObject->GetTransform()->GetPosition();
	float moveX = PizzaBox::InputManager::GetAxis("Horizontal");
	float moveY = PizzaBox::InputManager::GetAxis("Vertical");
	float moveZ = PizzaBox::InputManager::GetAxis("Depth");
	
	gameObject->GetTransform()->Translate(gameObject->GetTransform()->GetRight() * 15.0f * moveX * PizzaBox::Time::RealDeltaTime());
	gameObject->GetTransform()->Translate(gameObject->GetTransform()->GetUp() * 15.0f * moveY * PizzaBox::Time::RealDeltaTime());
	gameObject->GetTransform()->Translate(gameObject->GetTransform()->GetForward() * -15.0f * moveZ * PizzaBox::Time::RealDeltaTime());

	//----------------------------------------------------------------------------------//

	PizzaBox::Euler currentRotation = gameObject->GetTransform()->GetRotation();
	float rotateX = PizzaBox::InputManager::GetAxis("RotateX");
	float rotateY = PizzaBox::InputManager::GetAxis("RotateY");
	PizzaBox::Euler rotateAxis = PizzaBox::Euler(90.0f * rotateX, 90.0f * rotateY, 0.0f) * PizzaBox::Time::RealDeltaTime();
	gameObject->GetTransform()->Rotate(rotateAxis);

	//Get rid of any Z rotation
	currentRotation = gameObject->GetTransform()->GetRotation();
	currentRotation.z = 0.0f;
	gameObject->GetTransform()->SetRotation(currentRotation);

	//----------------------------------------------------------------------------------//

	if(PizzaBox::InputManager::GetButtonDown("ResetPos")){
		gameObject->GetTransform()->SetPosition(PizzaBox::Vector3(0.0f, 0.0f, 0.0f));
	}

	if(PizzaBox::InputManager::GetButtonDown("ResetRot")){
		gameObject->GetTransform()->SetRotation(PizzaBox::Euler(0.0f, 0.0f, 0.0f));
	}

	if(PizzaBox::InputManager::GetButtonDown("ResetScale")){
		gameObject->GetTransform()->SetScale(PizzaBox::Vector3(1.0f, 1.0f, 1.0f));
	}

	if(PizzaBox::InputManager::GetButtonDown("Print")){
		PizzaBox::Debug::Log("Vectors: ");
		PizzaBox::Debug::Log("Position: " + gameObject->GetTransform()->GetPosition().ToString());
		PizzaBox::Debug::Log("Forward: " + gameObject->GetTransform()->GetForward().ToString());
		PizzaBox::Debug::Log("Right: " + gameObject->GetTransform()->GetRight().ToString());
		PizzaBox::Debug::Log("Up: " + gameObject->GetTransform()->GetUp().ToString());
		PizzaBox::Debug::Log("--------------------------------------------");
	}

	if(PizzaBox::InputManager::GetButtonDown("Exit")){
		PizzaBox::GameManager::Stop();
	}

	if(PizzaBox::InputManager::GetButtonDown("PauseToggle")){
		toggle = !toggle;

		if(toggle == true){
			PizzaBox::Time::SetTimeScale(0.0f);
		}else if (toggle == false){
			PizzaBox::Time::SetTimeScale(1.0f);
		}
	}
}

void CharacterController::OnDestroy(){
}