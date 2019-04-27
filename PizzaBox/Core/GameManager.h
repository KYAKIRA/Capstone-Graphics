#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "GameInterface.h"
#include "Graphics/RenderEngine.h"

//We can rename this namespace to whatever
//I just figured an actual name would be better than something generic like "Engine"
namespace PizzaBox{
	class GameManager{
	public:
		static void Run(GameInterface* gameInterface_);
		static void Stop();
		static std::string Name();

		static constexpr unsigned int version = 2019'04'16;

	private:
		GameManager();
		~GameManager();

		GameManager(const GameManager&) = delete;
		GameManager(GameManager&&) = delete;
		GameManager& operator=(const GameManager&) = delete;
		GameManager& operator=(GameManager&&) = delete;

		static GameManager* instance;

		GameInterface* gameInterface;
		bool isRunning;

		bool Initialize(GameInterface* gameInterface_);
		void Destroy();
		void RunGameLoop();
	};
}

#endif //!GAME_MANAGER_H