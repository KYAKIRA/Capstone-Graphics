#include "LuaScript.h"

#include "Core/FileSystem.h"
#include "Tools/Debug.h"
#include "Tools/LuaManager.h"

using namespace PizzaBox;

LuaScript::LuaScript(const std::string& filePath_) : Resource(filePath_), scriptName(){
}

LuaScript::~LuaScript(){
}

bool LuaScript::Load(){
	if(FileSystem::FileExists(fileName) == false){
		Debug::LogError("Script file [" + fileName + "] does not exist!", __FILE__, __LINE__);
		return false;
	}

	std::string fileContents = FileSystem::ReadFileToString(fileName);
	if(fileContents.empty()){
		Debug::LogError("Script file [" + fileName + "] was empty!", __FILE__, __LINE__);
		return false;
	}

	auto fileContentsInLines = FileSystem::ReadFile(fileName);
	std::string firstLine = fileContentsInLines[0];
	scriptName = firstLine.substr(0, firstLine.find(" "));

	return LuaManager::EnableScript(scriptName, fileContents);
}

void LuaScript::Unload(){
	LuaManager::DisableScript(scriptName);
	scriptName = "";
}