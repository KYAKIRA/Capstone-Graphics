#ifndef LUA_MANAGER_H
#define LUA_MANAGER_H

#include <lua.hpp>
#include <rttr/registration.h>

namespace PizzaBox{
	class LuaManager{
	public:
		static bool Initialize();
		static void Destroy();

		static bool EnableScript(const std::string& scriptName_, const std::string& script_);
		static void DisableScript(const std::string& scriptName_);

		template <typename... ARGS>
		static void CallScriptFunction(const std::string& script_, const std::string& function_, ARGS&... args_){
			if(std::find(enabledScripts.begin(), enabledScripts.end(), script_) == enabledScripts.end()){
				Debug::LogError("Tried to call function on unloaded script [" + script_ + "]!", __FILE__, __LINE__);
				return;
			}

			lua_getglobal(state, script_.c_str());
			if(lua_type(state, -1) != LUA_TTABLE){
				Debug::LogError("could not find Lua table [" + script_ + "]!", __FILE__, __LINE__);
				return;
			}

			lua_pushstring(state, function_.c_str());
			lua_gettable(state, -2);

			//lua_getglobal(state, functionName_.c_str());
			if(lua_type(state, -1) != LUA_TFUNCTION){
				Debug::LogError("Could not find Lua function [" + function_ + "]!", __FILE__, __LINE__);
				return;
			}

			int numArgs = AddToLuaStack(args_...);
			int result = lua_pcall(state, numArgs, 0, 0);
			if(result != LUA_OK){
				Debug::LogError("Call to Lua function [" + function_ + "] was not completed! Lua Error: " + lua_tostring(state, -1), __FILE__, __LINE__);
			}
		}

		template <typename... ARGS>
		static void CallGlobalScriptFunction(std::string functionName_, ARGS&... args_){
			lua_getglobal(state, functionName_.c_str());
			if(lua_type(state, -1) != LUA_TFUNCTION){
				Debug::LogError("Could not find Lua function [" + functionName_ + "]!", __FILE__, __LINE__);
				return;
			}

			int numArgs = AddToLuaStack(args_...);
			int result = lua_pcall(state, numArgs, 0, 0);
			if(result != LUA_OK){
				Debug::LogError("Call to Lua function [" + functionName_ + "] was not completed! Lua Error: " + lua_tostring(state, -1), __FILE__, __LINE__);
			}
		}

	private:
		static std::vector<std::string> enabledScripts;

		static std::vector<union PassByValue> argValues;
		static std::vector<std::string> stringArgValues;

		static int CreateUserData(lua_State* L_);
		static int DestroyUserData(lua_State* L_);
		static int InvokeFuncOnUserData(lua_State* L_);
		static int IndexUserData(lua_State* L_);
		static int NewIndexUserData(lua_State* L_);
		static int InvokeStaticMethod(lua_State* L_);
		static int InvokeUNM(lua_State* L_);
		static int InvokeAdd(lua_State* L_);
		static int InvokeSub(lua_State* L_);
		static int InvokeMul(lua_State* L_);
		static int InvokeDiv(lua_State* L_);

		static std::string MetaTableName(const rttr::type& type_);

		static lua_State* state;

		static bool BindGlobalMethods();
		static bool BindClasses();
		static int InvokeMethod(const rttr::method& methodToInvoke_, const rttr::instance& object_);

		static rttr::argument LuaToNativeArg(const rttr::type& nativeType_, int luaStackArgIndex_, int luaArgType_, int i_);
		static int ToLuaValue(rttr::variant& result_);
		static int ToLuaValue(rttr::variant& result_, const std::string methodName_);
		static int CreateUserDataFromVariant(rttr::variant& v_);

		static int CallGlobalFromLua(lua_State* L_);

		static int AddToLuaStack();

		template <typename T>
		inline static int AddToLuaStack(T& value_){
			rttr::type typeOfT = rttr::type::get<T>();
			if(typeOfT.is_class()){
				//Pass by reference if the value is a class
				rttr::variant v(&value_);
				return ToLuaValue(v);
			}

			//Otherwise pass by value
			rttr::variant v(value_);
			return ToLuaValue(v);
		}

		template <typename T, typename... T2>
		inline static int AddToLuaStack(T& value_, T2&... moreArgs_){
			return AddToLuaStack(value_) + AddToLuaStack(moreArgs_...);
		}
	};
}

#endif //!LUA_MANAGER_H