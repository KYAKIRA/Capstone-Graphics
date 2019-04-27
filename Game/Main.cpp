#include <iostream>

#include <Core/GameManager.h>
#include <Tools/Debug.h>
#include "Game.h"

int main(int argc, char* argv[]){
	//This will cause compilation to fail for a 64-bit build
	//static_assert(sizeof(void*) == 4, "This program is not ready for 64-bit build");
	static_assert(PizzaBox::GameManager::version == 2019'04'16, "Invalid engine version!");

	//Create the Game
	GamePackage::Game* game = new GamePackage::Game("ProtoType");

	try{
		//Runs the game through the engine
		PizzaBox::GameManager::Run(game);
	}catch(std::exception& e){
		PizzaBox::Debug::DisplayFatalErrorMessage("Unhandled Exception!", e.what());
	}catch(std::string& e){
		PizzaBox::Debug::DisplayFatalErrorMessage("Unhandled Exception!", e);
	}catch(int e){
		PizzaBox::Debug::DisplayFatalErrorMessage("Unhandled Exception!", "Exception Code: " + std::to_string(e));
	}catch(...){
		PizzaBox::Debug::DisplayFatalErrorMessage("Unhandled Exception!", "Unknown exception type!");
	}

	//Delete the game
	delete game;
	game = nullptr;

	//This is to keep the console from closing immediately in Debug Mode
	//We don't really need the pre-processor statements since there is no console
	//in the Release configuration, but it's a bit cleaner this way
#ifdef _DEBUG
	std::cout << "\nThe program is now complete. Press enter to exit." << std::endl;
	std::cin.get();
#endif //_DEBUG
	return 0;
}