#ifndef ENGINE_STATS_H
#define ENGINE_STATS_H

#include <map>

namespace PizzaBox{
	class EngineStats{
	public:
		static bool Initialize();
		static void Destroy();

		static void Update(float deltaTime_);

		static float GetFloat(const std::string& key_);
		static long long GetInt(const std::string& key_);
		static std::string GetString(const std::string& key_);

		static void AddToFloat(const std::string& key_, float add_);
		static void AddToInt(const std::string& key_, long long add_);

		static void SetFloat(const std::string& key_, float value_);
		static void SetInt(const std::string& key_, long long value_);
		static void SetString(const std::string& key_, const std::string& value_);

		//Delete unwanted compiler-generated constructors, destructors and assignment operators
		EngineStats() = delete;
		EngineStats(const EngineStats&) = delete;
		EngineStats(EngineStats&&) = delete;
		EngineStats& operator=(const EngineStats&) = delete;
		EngineStats& operator=(EngineStats&&) = delete;
		~EngineStats() = delete;

	private:
		static float updateTimer;
		static float fpsCurrent;
		static int frameCount;

		static std::map<std::string, float> floatStats;
		static std::map<std::string, long long> intStats;
		static std::map<std::string, std::string> stringStats;
	};
}

#endif //!ENGINE_STATS_H