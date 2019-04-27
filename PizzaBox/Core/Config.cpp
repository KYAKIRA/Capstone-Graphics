#include "Config.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include "../Tools/Debug.h"
#include "FileSystem.h"

using namespace PizzaBox;

std::vector<ConfigFile> Config::files = std::vector<ConfigFile>();

bool Config::Initialize(){
	//Create all necessary config options and set default values for them
	SetDefaults();

	for(ConfigFile& file : files){
		std::fstream configFile;
		configFile.open(file.fileName, std::ios::in || std::ios::out);

		if(configFile.is_open()){
			//If the file was opened succesfully then we know that the file exists
			configFile.close();
			//Read the settings from the INI file
			ReadINI(file.fileName);
			//Rewrite the INI file just in case
			WriteINI(file.fileName);
		}else{
			configFile.close();
			//If the file doesn't exist then rewrite it
			WriteINI(file.fileName);
		}
	}

	return true;
}

void Config::Destroy(){
	files.clear();
}

void Config::SetDefaults(){
	CreateConfigFile("EngineConfig.ini");
	CreateConfigSection("EngineConfig.ini", "EngineSettings");
	AddConfig("EngineConfig.ini", "EngineSettings", "MaxAudioChannels", 1024);

	CreateConfigFile("UserConfig.ini");
	CreateConfigSection("UserConfig.ini", "SystemSettings");
	AddConfig("UserConfig.ini", "SystemSettings", "WindowX", 800);
	AddConfig("UserConfig.ini", "SystemSettings", "WindowY", 600);
	AddConfig("UserConfig.ini", "SystemSettings", "Fullscreen", false);
	AddConfig("UserConfig.ini", "SystemSettings", "Borderless", false);
	AddConfig("UserConfig.ini", "SystemSettings", "TargetFPS", 60);
	AddConfig("UserConfig.ini", "SystemSettings", "VSYNC", 1);

	CreateConfigSection("UserConfig.ini", "AudioSettings");
	AddConfig("UserConfig.ini", "AudioSettings", "MasterVolume", 1.0f);
	AddConfig("UserConfig.ini", "AudioSettings", "SFXVolume", 1.0f);
	AddConfig("UserConfig.ini", "AudioSettings", "MusicVolume", 1.0f);

	CreateConfigSection("UserConfig.ini", "GameSettings");
}

void Config::ReadINI(std::string file_){
	ConfigFile* file = nullptr;

	for(ConfigFile& cf : files){
		if(cf.fileName == file_){
			file = &cf;
			break;
		}
	}

	_ASSERT(file != nullptr);

	std::map<std::string, std::map<std::string, std::string>> records;
	FileSystem::ReadRecords(file_, records);

	for(auto& section : file->sections){
		for(const auto& value : records){
			if(section.first == value.first){
				for(const auto& innerValue : value.second){
					//If we find the key in ints then convert the value we found to an int and set it
					if(section.second.ints.find(innerValue.first) != section.second.ints.end()){
						section.second.ints.find(innerValue.first)->second = stoi(innerValue.second);
						continue;
					}

					//If we find the key in bools then convert the value we found to an int and set it
					if(section.second.bools.find(innerValue.first) != section.second.bools.end()){
						section.second.bools.find(innerValue.first)->second = stoi(innerValue.second);
						continue;
					}

					//If we find the key in floats then convert the value we found to an int and set it
					if(section.second.floats.find(innerValue.first) != section.second.floats.end()){
						section.second.floats.find(innerValue.first)->second = stof(innerValue.second);
						continue;
					}

					//If we find the key in strings then set it
					if(section.second.strings.find(innerValue.first) != section.second.strings.end()){
						section.second.strings.find(innerValue.first)->second = innerValue.second;
						continue;
					}

					//If we didn't find the value, treat it as an int
					try{
						int val = stoi(innerValue.second);
						section.second.ints[innerValue.first] = val;
						continue;
					}catch(...){
					}

					//If it couldn't be converted to an int, just treat it as a string
					section.second.strings[innerValue.first] = innerValue.second;
				}
			}
		}
	}
}

void Config::WriteINI(std::string file_){
	for(ConfigFile& file : files){
		if(file.fileName != file_){
			continue;
		}

		std::map<std::string, std::map<std::string, std::string>> records;

		for(auto& section : file.sections){
			records.insert(std::make_pair(section.first, std::map<std::string, std::string>()));

			for(auto const& i : section.second.ints){
				for(auto& v : records){
					if(v.first == section.first){
						v.second.insert(std::make_pair(i.first, std::to_string(i.second)));
					}
				}
			}

			for(auto const& b : section.second.bools){
				for(auto& v : records){
					if(v.first == section.first){
						v.second.insert(std::make_pair(b.first, std::to_string(b.second)));
					}
				}
			}

			for(auto const& f : section.second.floats){
				for(auto& v : records){
					if(v.first == section.first){
						v.second.insert(std::make_pair(f.first, std::to_string(f.second)));
					}
				}
			}

			for(auto const& s : section.second.strings){
				for(auto& v : records){
					if(v.first == section.first){
						v.second.insert(std::make_pair(s.first, s.second));
					}
				}
			}
		}

		FileSystem::WriteRecords(file.fileName, records, FileSystem::WriteType::clear);
		return;
	}

	Debug::LogError("Attempted to write an invalid config file!");
}

int Config::GetInt(std::string config_){
	//Converts the key to lowercase so that searching is not case sensitive
	std::transform(config_.begin(), config_.end(), config_.begin(), ::tolower);

	for(ConfigFile& file : files){
		for(auto& section : file.sections){
			auto search = section.second.ints.find(config_);
			if(search != section.second.ints.end()){
				return search->second;
			}
		}
	}

	//This will happen if you search for a key that doesn't exist
	Debug::LogError("Attempted to access a config setting that doesn't exist!", __FILE__, __LINE__);
	return 0;
}

bool Config::GetBool(std::string config_){
	//Converts the key to lowercase so that searching is not case sensitive
	std::transform(config_.begin(), config_.end(), config_.begin(), ::tolower);

	for(ConfigFile& file : files){
		for(auto& section : file.sections){
			auto search = section.second.bools.find(config_);
			if(search != section.second.bools.end()){
				return search->second;
			}
		}
	}

	//This will happen if you search for a key that doesn't exist
	Debug::LogError("Attempted to access a config setting that doesn't exist!", __FILE__, __LINE__);
	return false;
}

float Config::GetFloat(std::string config_){
	//Converts the key to lowercase so that searching is not case sensitive
	std::transform(config_.begin(), config_.end(), config_.begin(), ::tolower);

	for(ConfigFile& file : files){
		for(auto& section : file.sections){
			auto search = section.second.floats.find(config_);
			if(search != section.second.floats.end()){
				return search->second;
			}
		}
	}

	//This will happen if you search for a key that doesn't exist
	Debug::LogError("Attempted to access a config setting that doesn't exist!", __FILE__, __LINE__);
	return 0.0f;
}

std::string Config::GetString(std::string config_){
	//Converts the key to lowercase so that searching is not case sensitive
	std::transform(config_.begin(), config_.end(), config_.begin(), ::tolower);

	for(ConfigFile& file : files){
		for(auto& section : file.sections){
			auto search = section.second.strings.find(config_);
			if(search != section.second.strings.end()){
				return search->second;
			}
		}
	}

	//This will happen if you search for a key that doesn't exist
	Debug::LogError("Attempted to access a config setting that doesn't exist!", __FILE__, __LINE__);
	return "";
}

void Config::SetInt(std::string config_, int value_){
	std::transform(config_.begin(), config_.end(), config_.begin(), ::tolower);

	for(ConfigFile& file : files){
		for(auto& section : file.sections){
			auto search = section.second.ints.find(config_);
			if(search != section.second.ints.end()){
				search->second = value_;
				WriteINI(file.fileName);
				return;
			}
		}
	}

	//This will happen if the key doesn't exist
	AddConfig("UserConfig.ini", "GameSettings", config_, value_);
}

void Config::SetBool(std::string config_, bool value_){
	std::transform(config_.begin(), config_.end(), config_.begin(), ::tolower);

	for(ConfigFile& file : files){
		for(auto& section : file.sections){
			auto search = section.second.bools.find(config_);
			if(search != section.second.bools.end()){
				search->second = value_;
				WriteINI(file.fileName);
				return;
			}
		}
	}

	//This will happen if the key doesn't exist
	AddConfig("UserConfig.ini", "GameSettings", config_, value_);
}

void Config::SetFloat(std::string config_, float value_){
	std::transform(config_.begin(), config_.end(), config_.begin(), ::tolower);

	for(ConfigFile& file : files){
		for(auto& section : file.sections){
			auto search = section.second.floats.find(config_);
			if(search != section.second.floats.end()){
				search->second = value_;
				WriteINI(file.fileName);
				return;
			}
		}
	}

	//This will happen if the key doesn't exist
	AddConfig("UserConfig.ini", "GameSettings", config_, value_);
}

void Config::SetString(std::string config_, std::string value_){
	std::transform(config_.begin(), config_.end(), config_.begin(), ::tolower);

	for(ConfigFile& file : files){
		for(auto& section : file.sections){
			auto search = section.second.strings.find(config_);
			if(search != section.second.strings.end()){
				search->second = value_;
				WriteINI(file.fileName);
				return;
			}
		}
	}

	//This will happen if the key doesn't exist
	AddConfig("UserConfig.ini", "GameSettings", config_, value_);
}

bool Config::SettingExists(std::string config_){
	//Converts the key to lowercase so that searching is not case sensitive
	std::transform(config_.begin(), config_.end(), config_.begin(), ::tolower);

	for(ConfigFile& file : files){
		for(auto& section : file.sections){
			auto search = section.second.ints.find(config_);
			if(search != section.second.ints.end()){
				return true;
			}

			auto search2 = section.second.bools.find(config_);
			if(search2 != section.second.bools.end()){
				return true;
			}

			auto search3 = section.second.floats.find(config_);
			if(search3 != section.second.floats.end()){
				return true;
			}

			auto search4 = section.second.strings.find(config_);
			if(search4 != section.second.strings.end()){
				return true;
			}
		}
	}

	return false;
}

void Config::CreateConfigFile(std::string file_){
	files.push_back(ConfigFile(file_));
}

void Config::CreateConfigSection(std::string file_, std::string section_){
	for(ConfigFile& file : files){
		if(file.fileName == file_){
			file.sections.emplace(section_, ConfigSection());
		}
	}
}

void Config::AddConfig(std::string file_, std::string section_, std::string key_, int value_){
	for(ConfigFile& file : files){
		if(file.fileName == file_){
			for(auto& section : file.sections){
				if(section.first == section_){
					std::transform(key_.begin(), key_.end(), key_.begin(), ::tolower);
					section.second.ints.emplace(key_, value_);
					return;
				}
			}
		}
	}
}

void Config::AddConfig(std::string file_, std::string section_, std::string key_, bool value_){
	for(ConfigFile& file : files){
		if(file.fileName == file_){
			for(auto& section : file.sections){
				if(section.first == section_){
					std::transform(key_.begin(), key_.end(), key_.begin(), ::tolower);
					section.second.bools.emplace(key_, value_);
					return;
				}
			}
		}
	}
}

void Config::AddConfig(std::string file_, std::string section_, std::string key_, float value_){
	for(ConfigFile& file : files){
		if(file.fileName == file_){
			for(auto& section : file.sections){
				if(section.first == section_){
					std::transform(key_.begin(), key_.end(), key_.begin(), ::tolower);
					section.second.floats.emplace(key_, value_);
					return;
				}
			}
		}
	}
}

void Config::AddConfig(std::string file_, std::string section_, std::string key_, std::string value_){
	for(ConfigFile& file : files){
		if(file.fileName == file_){
			for(auto& section : file.sections){
				if(section.first == section_){
					std::transform(key_.begin(), key_.end(), key_.begin(), ::tolower);
					section.second.strings.emplace(key_, value_);
					return;
				}
			}
		}
	}
}