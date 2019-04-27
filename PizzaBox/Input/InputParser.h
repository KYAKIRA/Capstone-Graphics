#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <string>

namespace PizzaBox{
	class InputParser{
	public: 
		static bool ParseInputBinds(const std::string& defaultBindFile_, const std::string& customBindFile_);

		//Delete unwanted compiler generated constructors, destructors and assignment operators
		InputParser() = delete;
		InputParser(const InputParser&) = delete;
		InputParser(InputParser&&) = delete;
		InputParser& operator=(const InputParser&) = delete;
		InputParser& operator=(InputParser&&) = delete;
		~InputParser() = delete;

	private: 
		static bool ParseButtonInput(const std::string& entry_, const std::string& name_);
		static bool ParseAxisInput(const std::string& entry_, const std::string& name_);
	}; 
} 

#endif //!INPUT_PARSER_H