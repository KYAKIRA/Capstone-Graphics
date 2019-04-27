#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "Resource/Resource.h"

namespace PizzaBox{
	class LuaScript : public Resource{
	public:
		LuaScript(const std::string& filePath_);
		virtual ~LuaScript() override;

		virtual bool Load() override;
		virtual void Unload() override;

	private:
		std::string scriptName;
	};
}

#endif //!LUA_SCRIPT_H