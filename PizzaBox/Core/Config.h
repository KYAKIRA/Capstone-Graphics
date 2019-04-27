#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <map>

namespace PizzaBox{
	struct ConfigSection{
		std::map<std::string, int> ints;
		std::map<std::string, bool> bools;
		std::map<std::string, float> floats;
		std::map<std::string, std::string> strings;
	};

	struct ConfigFile{
		ConfigFile(std::string fileName_) : fileName(fileName_){}
		~ConfigFile(){}

		std::string fileName;
		std::map<std::string, ConfigSection> sections;
	};

	class Config{
	public:
		static bool Initialize();
		static void Destroy();

		static int GetInt(std::string config_);
		static bool GetBool(std::string config_);
		static float GetFloat(std::string config_);
		static std::string GetString(std::string config_);
		static void SetInt(std::string config_, int value_);
		static void SetBool(std::string config_, bool value_);
		static void SetFloat(std::string config_, float value_);
		static void SetString(std::string config_, std::string value_);

		static bool SettingExists(std::string config_);

	private:
		static std::vector<ConfigFile> files;

		static void SetDefaults();
		static void ReadINI(std::string file_);
		static void WriteINI(std::string file_);

		static void CreateConfigFile(std::string file_);
		static void CreateConfigSection(std::string file_, std::string section);
		static void AddConfig(std::string file_, std::string section_, std::string key_, int value_);
		static void AddConfig(std::string file_, std::string section_, std::string key_, bool value_);
		static void AddConfig(std::string file_, std::string section_, std::string key_, float value_);
		static void AddConfig(std::string file_, std::string section_, std::string key_, std::string value_);
	};
}

#endif //!CONFIG_H