#ifndef SCRIPT_MANAGER_H
#define SCRIPT_MANAGER_H

#include <vector>

#include "Script.h"

namespace PizzaBox{
	class ScriptManager{
	public:
		static bool Initialize();
		static void Destroy();

		static void RegisterScript(Script* script_);
		static void UnregisterScript(Script* script_);

		static void Update(const float deltaTime_);

	private:
		static std::vector<Script*> scripts;
	};
}

#endif //!SCRIPT_MANAGER_H