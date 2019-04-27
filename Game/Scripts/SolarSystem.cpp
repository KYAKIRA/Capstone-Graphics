#include "SolarSystem.h"

#include <Physics/Rigidbody.h>
#include <Object/GameObject.h>

using namespace GamePackage;

void SolarSystem::OnStart() {
	Script::OnStart();
}

void SolarSystem::Update(const float deltaTime_) {

	ApplyGravity(deltaTime_);
}


void SolarSystem::OnDestroy() {

}

void SolarSystem::ApplyGravity(const float deltaTime_) {
	//variable
	PizzaBox::Vector3 Direction, Fg, Nv, oldAccel;
	double Distance, F;
	double G = 6.67408e-11;			//Gravitational constant

	for (unsigned int i = 0; i < planets.size(); i++) {
		oldAccel = PizzaBox::Vector3();
		for (unsigned int j = 0; j < planets.size(); j++) {
			if (i == j) {
				continue;
			}

			Direction = planets[j]->GetTransform()->GetPosition() - planets[i]->GetTransform()->GetPosition();					//Direction
			Distance = sqrt(Direction.x * Direction.x + Direction.y * Direction.y + Direction.z * Direction.z);					//Distance(r)

			if (Distance > 1 || Distance < 0) {
				//Normailized vector
				Nv = Direction / static_cast<float>(Distance);	
				//Fgravity	
				F = (G * planets[i]->GetComponent<PizzaBox::Rigidbody>()->GetMass() * planets[j]->GetComponent<PizzaBox::Rigidbody>()->GetMass()) / (Distance * Distance);
				//Fg = Force of gravity
				Fg = Nv * static_cast<float>(F);
				oldAccel += Fg / planets[i]->GetComponent<PizzaBox::Rigidbody>()->GetMass() / 10000000;
				
			}// end of if Distance
		}// end of for j
		planets[i]->GetComponent<PizzaBox::Rigidbody>()->SetAcceleration(oldAccel);
	}//end of for i
}