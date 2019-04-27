#include "Random.h"

#include <cstdlib>
#include <chrono>

#include <rttr/registration.h>

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Random>("Random")
		.method("SetSeed", &Random::SetSeed)
		.method("Range", static_cast<int(*)(int, int)>(&Random::Range))
		.method("Range", static_cast<float(*)(float, float)>(&Random::Range))
		.method("RandomColor", &Random::RandomColor);
}
#pragma warning( pop )

void Random::SetSeed(){
	srand(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
}

int Random::Range(const int min, const int max){
	return rand() % max + min;
}

float Random::Range(const float min, const float max){
	float num = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	return num;
}

Color Random::RandomColor(){
	float r = Range(0.0f, 1.0f);
	float b = Range(0.0f, 1.0f);
	float g = Range(0.0f, 1.0f);
	return Color(r, b, g);
}