#include "Profiler.h"

#include <chrono>
#include <algorithm>

#include "Debug.h"

using namespace PizzaBox;

Profiler::Profiler(const std::string name_) : name(name_), startTime(0), times(std::vector<double>()), isProfiling(false){
}

Profiler::~Profiler(){
	//double totalTime = 0;

	//Remove the highest and the lowest times before calculating the average
	//This algorithm is pretty slow so I would only recommend doing it if you REALLY need it
	//if(times.size() < 100){
	//	times.erase(std::min_element(times.begin(), times.end()));
	//	times.erase(std::max_element(times.begin(), times.end()));
	//}else{
	//	int fivePercent = times.size() / 20;

		//Erase the highest and lowest 5 percent
	//	for(int i = 0; i < fivePercent; i++){
	//		times.erase(std::min_element(times.begin(), times.end()));
	//		times.erase(std::max_element(times.begin(), times.end()));
	//	}
	//}

	//for(double t : times){
	//	totalTime += t;
	//}

	//double averageTime = totalTime / static_cast<double>(times.size());

	Debug::Log("Average Time for " + name + ": " + std::to_string(GetAverage()) + " ms", __FILE__, __LINE__);
	Debug::Log("Number of times " + name + " was ran: " + std::to_string(GetIterations()), __FILE__, __LINE__);
}

void Profiler::StartProfiling(){
	//Make sure we're not profiling
	_ASSERT(!isProfiling);

	startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	isProfiling = true;
}

double Profiler::EndProfiling(){
	//Make sure we ARE profiling
	_ASSERT(isProfiling);

	long long currentTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	double timeTaken = static_cast<double>(currentTime - startTime) / 1000000.0f; //Converts the change in time to milliseconds

	times.push_back(timeTaken);

	isProfiling = false;

	return timeTaken;
}

bool Profiler::IsProfiling(){
	return isProfiling;
}

double Profiler::GetAverage(){
	double totalTime = 0.0f;

	for(double t : times){
		totalTime += t;
	}

	return totalTime / static_cast<double>(GetIterations());
}

double Profiler::GetNewestValue(){
	_ASSERT(times.size() > 0);
	return times.back();
}

long long Profiler::GetIterations(){
	return times.size();
}