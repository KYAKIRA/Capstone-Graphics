#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <string>

namespace PizzaBox{
	class GameInterface{
	public:
		explicit GameInterface(const std::string name_) : name(name_){};
		virtual ~GameInterface(){};

		std::string name;

		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;
	};
}

#endif //!GAME_INTERFACE_H