#include "InputParser.h"

#include <map>
#include <algorithm>

#include "Core/FileSystem.h"
#include "Input/InputManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

bool InputParser::ParseInputBinds(const std::string& defaultBindFile_, const std::string& customBindFile_){
	std::map<std::string, std::map<std::string, std::string>> inputs;

	//We have a custom file, load binds from that
	if(FileSystem::FileExists(customBindFile_)){
		FileSystem::ReadRecords(customBindFile_, inputs);
		if(inputs.size() == 0){
			Debug::Log("Custom file is empty, using default bindings", __FILE__, __LINE__);
			FileSystem::ReadRecords(defaultBindFile_, inputs);
			//FileSystem::WriteRecords(customBindFile_, inputs, FileSystem::WriteType::overwrite);
		}
	}else{
		Debug::Log("Custom file doesn't exist, using default bindings", __FILE__, __LINE__);
		FileSystem::ReadRecords(defaultBindFile_, inputs);
		//FileSystem::WriteRecords(customBindFile_, inputs, FileSystem::WriteType::overwrite);
	}

	for(const auto& entry : inputs){
		if(entry.first == "InputButton"){
			for(const auto& key : entry.second){
				bool status = ParseButtonInput(key.second, key.first);
				if(status == false){
					Debug::LogError("Could not bind button inputs to " + key.first, __FILE__, __LINE__);
					return false;
				}
			}
		}else if(entry.first == "InputAxis"){
			for(const auto& key : entry.second){
				bool status = ParseAxisInput(key.second, key.first);
				if(status == false){
					Debug::LogError("Could not bind axis inputs to " + key.first, __FILE__, __LINE__);
					return false;
				}
			}
		}else{
			Debug::LogError("Could not parse Input bound keys! " + entry.first + " type not registered", __FILE__, __LINE__);
			return false;
		}
	}

	return true;
}

bool InputParser::ParseButtonInput(const std::string& entry_, const std::string& name_){
	//------------Handles Full Entry------------------------------------
	auto entryStart = entry_.find_first_of("(");
	#ifdef _DEBUG
	if(entryStart == -1){
		Debug::LogError("Could not find expected symbol in Input binds!", __FILE__, __LINE__);
		return false;
	}
	#endif //_DEBUG

	auto entryEnd = entry_.find_last_of(")");
	#ifdef _DEBUG
	if(entryEnd == -1){
		Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
		return false;
	}
	#endif //_DEBUG

	std::string fullEntry = entry_.substr(entryStart + 1, entryEnd - entryStart) + ",";
	//------------Handles Full Entry------------------------------------
	//------------Handles Args------------------------------------
	auto argsStart = fullEntry.find_first_of("=", entryStart + 1);
	#ifdef _DEBUG
	if(argsStart == -1){
		Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
		return false;
	}
	#endif //_DEBUG

	auto argsEnd = fullEntry.find_first_of(",", argsStart + 1);
	#ifdef _DEBUG
	if(argsEnd == -1){
		Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
		return false;
	}
	#endif //_DEBUG

	unsigned int args = stoi(fullEntry.substr(argsStart + 1, argsEnd - argsStart - 1)); 
	//------------Handles Args------------------------------------
	 
	//------------Handles Args and Button Creation ---------------
	//Creates Button
	Button* button = new Button();

	auto lastPoint = argsEnd;
	bool isLastArg = false;
	
	for(unsigned int i = 0; i < args; i++){
		if(i == args - 1){
			isLastArg = true;
		}

		//------------Handles Type------------------------------------
		auto typeStart = fullEntry.find_first_of("=", lastPoint + 1);
		#ifdef _DEBUG
		if(typeStart == -1){
			Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
			return false;
		}
		#endif //_DEBUG

		auto typeEnd = fullEntry.find_first_of(",", typeStart + 1);
		#ifdef _DEBUG
		if(typeEnd == -1){
			Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
			return false;
		}
		#endif //_DEBUG
		//Update lastPoint
		lastPoint = typeEnd;
		std::string type = fullEntry.substr(typeStart + 2, typeEnd - typeStart - 3);
		//------------Handles Type------------------------------------
		if(type == "Key"){ 
			auto Start = fullEntry.find_first_of("=", lastPoint + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			auto End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			std::string key;
			if(!isLastArg){
				key = fullEntry.substr(Start + 2, End - Start - 3);
			}else{
				key = fullEntry.substr(Start + 2, End - Start - 4);
			}

			//Update lastPoint
			lastPoint = End;
			//Adds to button
			button->SetKeyInput(SDL_GetKeyFromName(key.c_str()));
		}else if (type == "Mouse"){ 
			auto Start = fullEntry.find_first_of("=", lastPoint + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			auto End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			std::string Mouse;
			if(!isLastArg){
				Mouse = fullEntry.substr(Start + 1, End - Start - 1);
			}else{
				Mouse = fullEntry.substr(Start + 1, End - Start - 2);
			}

			//Update lastPoint
			lastPoint = End;
			//Adds to button
			button->SetMouseInput(std::stoi(Mouse));
		}else if (type == "Gamepad"){
			auto Start = fullEntry.find_first_of("=", lastPoint + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			auto End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			unsigned int Index = stoi(fullEntry.substr(Start + 1, End - Start - 1));
			//Update lastPoint
			lastPoint = End;

			//------------Handles Gamepad------------------------------------
			Start = fullEntry.find_first_of("=", lastPoint + 1);
			#ifdef _DEBUG
			if (Start == -1) {
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if (End == -1) {
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			std::string gpString;
			if(!isLastArg){
				gpString = fullEntry.substr(Start + 1, End - Start - 1);
			}else{
				gpString = fullEntry.substr(Start + 1, End - Start - 2);
			}

			unsigned int Gamepad = stoi(gpString);
			//------------Handles Gamepad------------------------------------

			//Update lastPoint
			lastPoint = End;
			//Adds to button
			button->SetGamepadInput(Index, Gamepad);
		}
	} 

	//Trims name of \t and Pushes Axis to InputManager
	auto trimmedName = name_;
	trimmedName.erase(std::remove(trimmedName.begin(), trimmedName.end(), '\t'), trimmedName.end());
	InputManager::AddButton(trimmedName, button);

	return true;
}

bool InputParser::ParseAxisInput(const std::string& entry_, const std::string& name_){
	//------------Handles Full Entry------------------------------------
	auto entryStart = entry_.find_first_of("(");
	#ifdef _DEBUG
	if(entryStart == -1){
		Debug::LogError("Could not find expected symbol in Input binds!", __FILE__, __LINE__);
		return false;
	}
	#endif //_DEBUG

	auto entryEnd = entry_.find_last_of(")");
	#ifdef _DEBUG
	if(entryEnd == -1){
		Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
		return false;
	}
	#endif //_DEBUG

	std::string fullEntry = entry_.substr(entryStart + 1, entryEnd - entryStart) + ",";
	//------------Handles Full Entry------------------------------------
	//------------Handles Args------------------------------------
	auto argsStart = fullEntry.find_first_of("=", entryStart + 1);
	#ifdef _DEBUG
	if(argsStart == -1){
		Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
		return false;
	}
	#endif //_DEBUG

	auto argsEnd = fullEntry.find_first_of(",", argsStart + 1);
	#ifdef _DEBUG
	if(argsEnd == -1){
		Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
		return false;
	}
	#endif //_DEBUG

	unsigned int args = stoi(fullEntry.substr(argsStart + 1, argsEnd - argsStart - 1));
	//------------Handles Args------------------------------------
	//------------Handles Args and Axis Creation ---------------
	//Creates Axis
	Axis* axis = new Axis();
	
	auto lastPoint = argsEnd;
	bool isLastArg = false;
	for(unsigned int i = 0; i < args; i++){
		if(i == args - 1){
			isLastArg = true;
		}

		//------------Handles Type------------------------------------
		auto typeStart = fullEntry.find_first_of("=", lastPoint + 1);
		#ifdef _DEBUG
		if(typeStart == -1){
			Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
			return false;
		}
		#endif //_DEBUG

		auto typeEnd = fullEntry.find_first_of(",", typeStart + 1);
		#ifdef _DEBUG
		if(typeEnd == -1){
			Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
			return false;
		}
		#endif //_DEBUG 
		std::string type = fullEntry.substr(typeStart + 2, typeEnd - typeStart - 3);

		//Update lastPoint
		lastPoint = typeEnd;
		//------------Handles Type------------------------------------
		if(type == "Key"){
			//------------Handles Key1------------------------------------
			auto Start = fullEntry.find_first_of("=", lastPoint + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			auto End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			std::string key1 = fullEntry.substr(Start + 2, End - Start - 3); 
			//------------Handles Key1------------------------------------
			//------------Handles Key2------------------------------------
			Start = fullEntry.find_first_of("=", End + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			std::string key2;
			if(!isLastArg){
				key2 = fullEntry.substr(Start + 2, End - Start - 3);
			}else{
				key2 = fullEntry.substr(Start + 2, End - Start - 4);
			}
			//Update lastPoint
			lastPoint = End;
			//------------Handles Key2------------------------------------
			//Adds to button
			axis->SetKeyInput(SDL_GetKeyFromName(key1.c_str()), SDL_GetKeyFromName(key2.c_str()));
		}else if (type == "Mouse"){
			auto Start = fullEntry.find_first_of("=", lastPoint + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			auto End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			std::string mouseString;
			if(!isLastArg){
				mouseString = fullEntry.substr(Start + 1, End - Start - 1);
			}else{
				mouseString = fullEntry.substr(Start + 1, End - Start - 2);
			}

			unsigned int mouse = stoi(mouseString);
			//Update lastPoint
			lastPoint = End;
			//Adds to button
			axis->SetMouseAxis(mouse);
		}else if (type == "Scroll"){
			auto Start = fullEntry.find_first_of("=", lastPoint + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			auto End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			std::string scrString;
			if(!isLastArg){
				scrString = fullEntry.substr(Start + 1, End - Start - 1);
			}else{
				scrString = fullEntry.substr(Start + 1, End - Start - 2);
			}

			unsigned int scroll = stoi(scrString);
			//Update lastPoint
			lastPoint = End;
			//Adds to button
			axis->SetScrollAxis(scroll);
		}else if (type == "GamepadAxis"){
			//------------Handles Index------------------------------------
			auto Start = fullEntry.find_first_of("=", lastPoint + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			auto End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			unsigned int Index = stoi(fullEntry.substr(Start + 1, End - Start - 1)); 
			//------------Handles Index------------------------------------
			//------------Handles GamepadAxis------------------------------------
			Start = fullEntry.find_first_of("=", End + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			std::string gpString;
			if(!isLastArg){
				gpString = fullEntry.substr(Start + 1, End - Start - 1);
			}else{
				gpString = fullEntry.substr(Start + 1, End - Start - 2);
			}

			unsigned int gamepadAxis = stoi(gpString);
			//------------Handles GamepadAxis------------------------------------

			//Update lastPoint
			lastPoint = End;
			//Adds to button
			axis->SetGamepadAxis(Index, gamepadAxis);
		}else if (type == "GamepadPad"){
			//------------Handles Index------------------------------------
			auto Start = fullEntry.find_first_of("=", lastPoint + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			auto End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			unsigned int Index = stoi(fullEntry.substr(Start + 1, End - Start - 1)); 
			//------------Handles Index------------------------------------
			//------------Handles Gamepad------------------------------------
			Start = fullEntry.find_first_of("=", End + 1);
			#ifdef _DEBUG
			if(Start == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			End = fullEntry.find_first_of(",", Start + 1);
			#ifdef _DEBUG
			if(End == -1){
				Debug::LogError("Could not find expected symbol in Input Binds!", __FILE__, __LINE__);
				return false;
			}
			#endif //_DEBUG

			std::string gpString;
			if(!isLastArg){
				gpString = fullEntry.substr(Start + 1, End - Start - 1);
			}else{
				gpString = fullEntry.substr(Start + 1, End - Start - 2);
			}

			unsigned int gamepad = stoi(gpString);
			//------------Handles Gamepad------------------------------------

			//Update lastPoint
			lastPoint = End;
			//Adds to button
			axis->SetGamepadPad(Index, gamepad);
		}
	}

	//Trims name of \t and Pushes Axis to InputManager
	auto trimmedName = name_;
	trimmedName.erase(std::remove(trimmedName.begin(), trimmedName.end(), '\t'), trimmedName.end());
	InputManager::AddAxis(trimmedName, axis);

	return true;
}