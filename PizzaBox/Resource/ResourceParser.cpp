#include "ResourceParser.h"

#include <map>

#include "ResourceManager.h"
#include "Animation/AnimModel.h"
#include "Animation/AnimClip.h"
#include "Audio/AudioResource.h"
#include "Core/FileSystem.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Models/Model.h"
#include "Graphics/Sky/SkyBoxResource.h"
#include "Graphics/Text/Font.h"
#include "Tools/LuaScript.h"
 
using namespace PizzaBox;

bool ResourceParser::ParseEngineResources(){
	std::map<std::string, std::map<std::string, std::string>> engineResources;
	FileSystem::ReadRecords("Resources/EngineResources.ini", engineResources);

	for(const auto& entry : engineResources){
		if(entry.first == "Shaders"){
			for(const auto& resource : entry.second){
				Shader* shader = ParseShader(resource.second);
				if(shader == nullptr){
					Debug::LogError("Could not parse resource " + resource.first);
					return false;
				}
				ResourceManager::AddResource(resource.first, shader);
			}
		}else if(entry.first == "PermanentShaders"){
			for(const auto& resource : entry.second){
				Shader* shader = ParseShader(resource.second);
				if(shader == nullptr){
					Debug::LogError("Could not parse resource " + resource.first);
					return false;
				}
				ResourceManager::AddPermanentResource(resource.first, shader);
			}
		}else{
			Debug::LogError("Could not parse engine resource of type " + entry.first + "!");
			return false;
		}
	}

	return true;
}

bool ResourceParser::ParseGameResources(){
	std::map<std::string, std::map<std::string, std::string>> gameResources;
	FileSystem::ReadRecords("Resources/GameResources.ini", gameResources);

	for(const auto& entry : gameResources){
		for(const auto& resource : entry.second){
			std::string filePath;
			auto pathStart = resource.second.find_first_of("\"") + 1;
			auto pathEnd = resource.second.find_last_of("\"") - 1;
			filePath = resource.second.substr(pathStart, pathEnd);

			if(entry.first == "Textures"){
				ResourceManager::AddResource(resource.first, new Texture(filePath));
			}else if(entry.first == "Skyboxes"){
				ResourceManager::AddResource(resource.first, new SkyBoxResource(filePath));
			}else if(entry.first == "Models"){
				ResourceManager::AddResource(resource.first, new Model(filePath));
			}else if(entry.first == "AnimModels"){
				ResourceManager::AddResource(resource.first, new AnimModel(filePath));
			}else if(entry.first == "AnimClips"){
				ResourceManager::AddResource(resource.first, new AnimClip(filePath));
			}else if(entry.first == "Fonts"){
				ResourceManager::AddResource(resource.first, new Font(filePath));
			}else if(entry.first == "Audio"){
				ResourceManager::AddResource(resource.first, new AudioResource(filePath));
			}else if(entry.first == "LuaScript"){
				ResourceManager::AddResource(resource.first, new LuaScript(filePath));
			}else if(entry.first == "Shaders"){
				Shader* shader = ParseShader(resource.second);
				if(shader == nullptr){
					Debug::LogError("Could not parse resource " + resource.first);
					return false;
				}
				ResourceManager::AddResource(resource.first, shader);
			}else if(entry.first == "PermanentTextures"){
				ResourceManager::AddPermanentResource(resource.first, new Texture(filePath));
			}else if(entry.first == "PermanentSkyboxes"){
				ResourceManager::AddPermanentResource(resource.first, new SkyBoxResource(filePath));
			}else if(entry.first == "PermanentModels"){
				ResourceManager::AddPermanentResource(resource.first, new Model(filePath));
			}else if(entry.first == "PermanentAnimModels"){
				ResourceManager::AddPermanentResource(resource.first, new AnimModel(filePath));
			}else if(entry.first == "PermanentAnimClips"){
				ResourceManager::AddPermanentResource(resource.first, new AnimClip(filePath));
			}else if(entry.first == "PermanentFonts"){
				ResourceManager::AddPermanentResource(resource.first, new Font(filePath));
			}else if(entry.first == "PermanentAudio"){
				ResourceManager::AddPermanentResource(resource.first, new AudioResource(filePath));
			}else if(entry.first == "PermanentLuaScripts"){
				ResourceManager::AddPermanentResource(resource.first, new LuaScript(filePath));
			}else if(entry.first == "PermanentShaders"){
				Shader* shader = ParseShader(resource.second);
				if(shader == nullptr){
					Debug::LogError("Could not parse resource " + resource.first);
					return false;
				}
				ResourceManager::AddPermanentResource(resource.first, shader);
			}else{
				Debug::LogError("Could not parse game resource of type " + entry.first + "!");
				return false;
			}
		}
	}

	return true;
}
 
Shader* ResourceParser::ParseShader(std::string entry){
	auto entryStart = entry.find_first_of("(");

	#ifdef _DEBUG
	if(entryStart == -1){
		Debug::LogError("Could not find expected symbol in Shader resource value!", __FILE__, __LINE__);
		return nullptr;
	}
	#endif //_DEBUG

	auto entryEnd = entry.find_last_of(")");

	#ifdef _DEBUG
	if(entryEnd == -1){
		Debug::LogError("Could not find expected symbol in Shader resource value!", __FILE__, __LINE__);
		return nullptr;
	}
	#endif //_DEBUG

	std::string fullEntry = entry.substr(entryStart + 1, entryEnd - entryStart);

	auto vertStart = fullEntry.find_first_of("=", entryStart + 1);

	#ifdef _DEBUG
	if(vertStart == -1){
		Debug::LogError("Could not find expected symbol in Shader resource value!", __FILE__, __LINE__);
		return nullptr;
	}
	#endif //_DEBUG

	auto vertEnd = fullEntry.find_first_of(",", vertStart + 1);
	#ifdef _DEBUG
	if(vertEnd == -1){
		Debug::LogError("Could not find expected symbol in Shader resource value!", __FILE__, __LINE__);
		return nullptr;
	}
	#endif //_DEBUG

	std::string vertPath = fullEntry.substr(vertStart + 2, vertEnd - vertStart - 3);

	auto fragStart = fullEntry.find_first_of("=", vertEnd + 1);
	#ifdef _DEBUG
	if(fragStart == -1){
		Debug::LogError("Could not find expected symbol in Shader resource value!", __FILE__, __LINE__);
		return nullptr;
	}
	#endif //_DEBUG

	auto fragEnd = fullEntry.find_first_of(",", fragStart + 1);
	bool fragIsEnd = false;
	if(fragEnd == -1){
		//If we don't find a comma after the fragPath, assume the frag path is the very end of the entry
		fragEnd = fullEntry.find_first_of(")", fragStart + 1);
		fragIsEnd = true;
		
		#ifdef _DEBUG
		if(fragEnd == -1){
			Debug::LogError("Could not find expected symbol in Shader resource value!", __FILE__, __LINE__);
			return nullptr;
		}
		#endif //_DEBUG
	}

	std::string fragPath = fullEntry.substr(fragStart + 2, fragEnd - fragStart - 3);
	if(fragIsEnd){
		return new Shader(vertPath, fragPath);
	}

	auto argsStart = fullEntry.find_first_of("=", fragEnd + 1);
	#ifdef _DEBUG
	if(argsStart == -1){
		Debug::LogError("Could not find expected symbol in Shader resource value!", __FILE__, __LINE__);
		return nullptr;
	}
	#endif //_DEBUG

	auto argsEnd = fullEntry.find_first_of(",", argsStart + 1);
	#ifdef _DEBUG
	if(argsEnd == -1){
		Debug::LogError("Could not find expected symbol in Shader resource value!", __FILE__, __LINE__);
		return nullptr;
	}
	#endif //_DEBUG

	unsigned int argCount = stoi(fullEntry.substr(argsStart + 1, argsEnd - argsStart - 1));

	std::vector<std::string> args = std::vector<std::string>();
	auto start = argsStart;
	auto end = argsEnd;
	for(unsigned int i = 0; i < argCount; i++){
		start = fullEntry.find_first_of("=", end + 1);
		#ifdef _DEBUG
		if(start == -1){
			Debug::LogError("Could not find expected symbol in Shader resource value!", __FILE__, __LINE__);
			return nullptr;
		}
		#endif //_DEBUG

		end = fullEntry.find_first_of(",", start + 1);
		if(end == -1){
			//We've hit the final argument? Search for a " and then cut that out
			end = fullEntry.find_last_of("\"");
			#ifdef _DEBUG
			if(end == -1){
				Debug::LogError("Could not find expected symbol in Shader resource value!", __FILE__, __LINE__);
				return nullptr;
			}
			#endif //_DEBUG

			args.push_back(fullEntry.substr(start + 2, end - start - 2));
		}else{
			args.push_back(fullEntry.substr(start + 2, end - start - 2));
		}
	}

	if(args.size() != argCount){
		Debug::LogError("An error occured while parsing Shader resource!");
		return nullptr;
	}

	return new Shader(vertPath, fragPath, args);
}