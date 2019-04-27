#include "GameManager.h"

#include <rttr/registration.h>

#include "Config.h"
#include "SceneManager.h"
#include "Time.h"
#include "Animation/AnimEngine.h"
#include "Audio/AudioManager.h"
#include "Graphics/Text/FontEngine.h"
#include "Input/InputManager.h"
#include "Physics/PhysicsEngine.h"
#include "Resource/ResourceManager.h"
#include "Script/ScriptManager.h"
#include "Tools/Debug.h"
#include "Tools/EngineStats.h"
#include "Tools/LuaManager.h"
#include "Tools/Random.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<GameManager>("GameManager")
		.method("Run", &GameManager::Run)
		.method("Stop", &GameManager::Stop)
		.method("Name", &GameManager::Name)
		.property_readonly("version", &GameManager::version);
}
#pragma warning( pop )

//Static instance
GameManager* GameManager::instance = nullptr;

//Initialize all member variables to default values here
//We don't necessarily need to do this for every variable, but it's generally a good idea to do it like this anyway
GameManager::GameManager() : gameInterface(nullptr), isRunning(false){
}

//We use Destroy to clean up the memory for this, so we don't need to put anything in here
GameManager::~GameManager(){
}

void GameManager::Run(GameInterface* gameInterface_){
	//GameManager must not already exist
	_ASSERT(instance == nullptr);
	//GameInterface must be a valid pointer
	_ASSERT(gameInterface_ != nullptr);

	try{
		instance = new GameManager();
		//If initialization is successful, run the game loop
		if(instance->Initialize(gameInterface_) == true){
			instance->RunGameLoop();
		}else{
			std::cout << std::endl << "Error: GameManager could not be initialized!" << std::endl;
		}
	}catch(std::exception& e){
		PizzaBox::Debug::DisplayFatalErrorMessage("Unhandled Exception!", e.what());
	}catch(std::string& e){
		PizzaBox::Debug::DisplayFatalErrorMessage("Unhandled Exception!", e);
	}catch(int e){
		PizzaBox::Debug::DisplayFatalErrorMessage("Unhandled Exception!", "Exception Code: " + std::to_string(e));
	}catch(...){
		PizzaBox::Debug::DisplayFatalErrorMessage("Unhandled Exception!", "Unknown exception type!");
	}

	//Cleanup the GameManager
	if(instance != nullptr){
		instance->Destroy();
		delete instance;
		instance = nullptr;
	}
}

void GameManager::Stop(){
	//GameManager must exist
	_ASSERT(instance != nullptr);

	instance->isRunning = false;
}

std::string GameManager::Name(){
	//GameManager must exist
	_ASSERT(instance != nullptr);
	//GameInterface must exist
	_ASSERT(instance->gameInterface != nullptr);

	return instance->gameInterface->name;
}

//If any initializations fail, stop and return false
//If everything works, return true
bool GameManager::Initialize(GameInterface* gameInterface_){
	//Make sure the GameInterface is a valid pointer
	//We already did this exact assertion in the Run function so this should never trigger
	//But you can never be too careful
	_ASSERT(gameInterface_ != nullptr);

	gameInterface = gameInterface_;

	//Initialize the Debug system
	if(Debug::Initialize() == false){
		std::cout << "Error: Debug System could not be initialized!" << std::endl;
		return false;
	}

	if(EngineStats::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "EngineStats could not be initialized!");
		return false;
	}

	//Initialize the Configu system
	if(Config::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "Config System could not be initialized!");
		return false;
	}

	//Initialize the ResourceManager
	if(ResourceManager::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "ResourceManager could not be initialized!");
		return false;
	}

	//Initialize the RenderEngine
	if(RenderEngine::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "RenderEngine could not be initialized!");
		return false;
	}

	//Initialize the AnimEngine
	if(AnimEngine::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "Animation System could not be initialized!");
		return false;
	}

	//Initialize the FontEngine
	if(FontEngine::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "FontEngine could not be initialized!");
		return false;
	}

	//Initialize the InputManager
	if(InputManager::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "InputManager could not be initialized!");
		return false;
	}

	//Initialize the Physics Engine
	if(PhysicsEngine::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "PhysicsEngine could not be initialized!");
		return false;
	}

	//Initialize the AudioManager
	if(AudioManager::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "AudioManager could not be initialized!");
		return false;
	}

	//Initialize Lua
	if(LuaManager::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "LuaManager could not be initialized!");
		return false;
	}

	//Initialize the ScriptManager
	if(ScriptManager::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "Script Manager could not be initialized!");
		return false;
	}

	//Initialize the Time System
	if(Time::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "Timer subsystems could not be initialized!");
		return false;
	}

	//Initialize the SceneManager
	//Since this needs to be the first engine subsystem we unload, it should be the last one we load
	if(SceneManager::Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "SceneManager could not be initialized!");
		return false;
	}
	
	//Seed our random number generation
	Random::SetSeed();

	//Load our permanent resources
	ResourceManager::LoadPermanentResources();

	//Initialize the GameInterface
	if(gameInterface->Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "GameInterface could not be initialized!");
		return false;
	}

	//In Debug mode, test loading all scenes
	//If you have this, you get to test all the scene initializations right away (at the expense of startup speed)
	//You should comment this out to speed things up for normal testing, but it can be useful for tracking down scene initialization errors
	#ifdef _DEBUG
	//if(SceneManager::TestLoadingAllScenes() == false){
	//	Debug::DisplayFatalErrorMessage("Initialization Error", "SceneManager could not load a certain scene!");
	//	return false;
	//}
	#endif //_DEBUG

	//Load the initial scene after initializing the GameInterface
	if(SceneManager::LoadInitialScene() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "SceneManager could not load the initial scene!");
		return false;
	}

	//Setting isRunning here makes sure the game loop will run
	isRunning = true;
	return true;
}

//Destroy and delete all engine subsystems
//Things should be destroyed/deleted in reverse order that they were created
//Example, the gameInterface depends on the whole engine, so it should be initialized last and destroyed first
void GameManager::Destroy(){
	//We don't delete the gameInterface here, we just destroy it
	if(gameInterface != nullptr){
		gameInterface->Destroy();
		gameInterface = nullptr;
	}

	//Unload our permanent resources
	ResourceManager::UnloadPermanentResources();

	//SceneManager needs to be unloaded before any other engine subsystems
	SceneManager::Destroy();
	ScriptManager::Destroy();
	LuaManager::Destroy();
	AudioManager::Destroy();
	PhysicsEngine::Destroy();
	InputManager::Destroy();
	FontEngine::Destroy();
	AnimEngine::Destroy();
	RenderEngine::Destroy();
	ResourceManager::Destroy();
	Config::Destroy();
	EngineStats::Destroy();
	Debug::Destroy();

	//The engine is no longer initialized, so we set this to false so that nobody can try to run the engine
	isRunning = false;
}

void GameManager::RunGameLoop(){
	//If isRunning is not true then initialization either never happened or failed
	//If it failed then this should never have been called in the first place
	_ASSERT(isRunning == true);

	Time::Start();

	//Main Game Loop
	while(isRunning){
		//Begin profiling the main game loop
		Debug::StartProfiling("Main Game Loop");

		//Update the timer for this frame
		//This should be the first thing that happens
		Time::UpdateFrameTicks();

		//Check all input for this frame
		//If a quit event is triggered, PollInput will call GameManager::Stop()
		//This should be the first thing that happens after updating the timer
		InputManager::PollInput();

		//Update the current scene
		//This should be done before any game logic
		if(SceneManager::Update() == false){
			//Don't bother doing anything else if something dies here
			Debug::DisplayFatalErrorMessage("SceneManager Error", "An error has occured while updating the SceneManager!");
			break;
		}

		//Update all physics objects in the current scene
		PhysicsEngine::Update(Time::DeltaTime());

		//Update Custom Game Logic
		ScriptManager::Update(Time::DeltaTime());

		//Update the Audio Engine
		AudioManager::Update();

		AnimEngine::Update(Time::DeltaTime());

		//Render all renderable objects in the current scene and draw the rendered frame to the window
		//This should be the last thing that happens before delaying the timer
		RenderEngine::Render();
		
		//Stop measuring the length of the game loop before the Timer delay
		Debug::EndProfiling("Main Game Loop");

		EngineStats::Update(Time::PureDeltaTime());

		//Wait the appropriate amount of time before starting the next frame
		//This should be the last thing that happens in the game loop
		Time::Delay();
	}
}