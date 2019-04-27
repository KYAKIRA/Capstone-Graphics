#include "ScriptManager.h"

using namespace PizzaBox;

std::vector<Script*> ScriptManager::scripts;

bool ScriptManager::Initialize(){
	return true;
}

void ScriptManager::Destroy(){
}

void ScriptManager::RegisterScript(Script* script_){
	_ASSERT(script_ != nullptr);
	scripts.push_back(script_);
}

void ScriptManager::UnregisterScript(Script* script_){
	_ASSERT(script_ != nullptr);
	scripts.erase(std::remove(scripts.begin(), scripts.end(), script_), scripts.end());
}

void ScriptManager::Update(const float deltaTime_){
	for(Script* script : scripts){
		if(script->GetEnable()){
			//Start the script
			if(!script->HasStarted()){
				script->OnStart();
				script->hasStarted = true;
			}

			//Handle Collision Enter
			while(!script->collisionEnterQueue.empty()){
				script->OnCollision(script->collisionEnterQueue.front());
				script->collisionEnterQueue.pop();
			}

			//Handle Continued Collisions
			while(!script->collisionStayQueue.empty()){
				script->OnCollisionStay(script->collisionStayQueue.front());
				script->collisionStayQueue.pop();
			}

			//Handle Collision Exits
			while(!script->collisionExitQueue.empty()){
				script->OnCollisionExit(script->collisionExitQueue.front());
				script->collisionExitQueue.pop();
			}

			//Update
			script->Update(deltaTime_);
		}
	}
}