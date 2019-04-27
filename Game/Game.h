#ifndef GAME_H
#define GAME_H

#include <Core/GameInterface.h>

//You can call the namespace whatever is appropriate for your game
namespace GamePackage{
	class Game : public PizzaBox::GameInterface{
	public:
		explicit Game(const std::string name_);
		~Game();

		bool Initialize();
		void Destroy();
	};
}

#endif //!GAME_H